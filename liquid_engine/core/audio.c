/**
 * Description:  Audio API Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 27, 2023
*/
#include "defines.h"
#include "core/audio.h"
#include "core/time.h"
#include "core/logging.h"
#include "core/memory.h"
#include "core/thread.h"
#include "core/math.h"
#include "core/internal.h"

global f32 global_master_volume = 0.05f;

LD_API void audio_set_master_volume( f32 volume ) {
    // f32 volume01 = clamp01( volume );
    // f32 logarithmic = logarithm10( volume01 ) * 20.0f;
    // global_master_volume = logarithmic;
    //
    global_master_volume = clamp01( volume );
}
LD_API f32 audio_query_master_volume(void) {
    return global_master_volume;
}

// TODO(alicia): this is currently temporary code!
// Platform-specific functions should be used only in liquid_platform!

#include <initguid.h>
#include <mmdeviceapi.h>
#include <dshow.h>
#include <audioclient.h>
#include <mmreg.h>

global IAudioClient*        global_audio_client        = NULL;
global IAudioRenderClient*  global_audio_render_client = NULL;
global IMMDeviceEnumerator* global_device_enumerator   = NULL;
global IMMDevice*           global_device              = NULL;

global WAVEFORMATEX global_format;

global UINT32 global_buffer_frame_count = 0;
global usize  global_buffer_size  = 0;

/// Defining required win32 functions.

#define AUDIO_DEFINE_FUNCTION( ret, name, ... )\
    typedef ret ___internal_##name##FN( __VA_ARGS__ );\
    ___internal_##name##FN* ___internal_##name = NULL

AUDIO_DEFINE_FUNCTION( HRESULT, CoCreateInstance, REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv );
#define CoCreateInstance ___internal_CoCreateInstance

AUDIO_DEFINE_FUNCTION( HRESULT, CoInitialize, LPVOID pvReserved );
#define CoInitialize ___internal_CoInitialize

AUDIO_DEFINE_FUNCTION( void, CoUninitialize, void );
#define CoUninitialize ___internal_CoUninitialize

AUDIO_DEFINE_FUNCTION( void, CoTaskMemFree, LPVOID pv );
#define CoTaskMemFree ___internal_CoTaskMemFree

/// For converting actually useful
/// time lengths to microsoft's bs.
#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MS  10000

internal const char* ___audio_wave_format_to_cstr( DWORD format_tag );

/// Convert audio frames to bytes.
internal force_inline maybe_unused
usize ___audio_buffer_frame_count_to_bytes( UINT32 buffer_frame_count ) {
    return buffer_frame_count * (global_format.nChannels * ( global_format.wBitsPerSample / 8 ));
}

/// Convert bytes to audio frames.
internal force_inline maybe_unused
UINT32 ___audio_buffer_bytes_to_frame_count( usize buffer_bytes ) {
    return buffer_bytes / ( global_format.nChannels * ( global_format.wBitsPerSample / 8 ) );
}

/// Lock the sound buffer.
/// Returns true if locking the buffer was successful.
/// out_total_buffer_size returns the total size of the buffer.
/// out_bytes_available returns how much space is free in the buffer.
/// out_frames_available returns how many frames are free in the buffer.
/// out_data returns a pointer to the buffer.
internal maybe_unused
b32 ___audio_lock_buffer( UINT32* out_frames_available, usize* out_bytes_available, BYTE** out_buffer ) {

    UINT32 buffer_frame_padding_count = 0;
    HRESULT hresult = global_audio_client->lpVtbl->GetCurrentPadding(
        global_audio_client, &buffer_frame_padding_count );
    if( FAILED( hresult ) ) {
        warn_log( "Audio -> failed to get current padding?" );
        return false;
    }

    if( buffer_frame_padding_count > global_buffer_frame_count ) {
        warn_log( "Audio -> buffer frame padding count was greater than buffer frame count?" );
        return false;
    }

    UINT32 buffer_frames_to_request =
        global_buffer_frame_count - buffer_frame_padding_count;

    BYTE* buffer = NULL;
    hresult = global_audio_render_client->lpVtbl->GetBuffer(
        global_audio_render_client, buffer_frames_to_request, &buffer );
    if( hresult != S_OK ) {
        warn_log( "Audio -> failed to retrieve buffer?" );
        return false;
    }
    if( !buffer ) {
        return false;
    }

    *out_bytes_available  = ___audio_buffer_frame_count_to_bytes( buffer_frames_to_request );
    *out_frames_available = buffer_frames_to_request;
    *out_buffer = buffer;

    return true;
}

/// Unlock locked buffer.
internal maybe_unused
void ___audio_unlock_buffer( UINT32 frames_locked ) {
    HRESULT hresult = global_audio_render_client->lpVtbl->ReleaseBuffer(
        global_audio_render_client, frames_locked, 0 );

    assert_log( SUCCEEDED( hresult ), "Failed to unlock audio buffer?" );
}

b32 audio_subsystem_initialize(void) {
    /// Load required functions.

    PlatformLibrary* ole32_lib = platform->library.open( "OLE32.DLL" );
    if( !ole32_lib ) {
        fatal_log( "Failed to open OLE32.DLL!" );
        return false;
    }

    #define AUDIO_LOAD_FUNCTION( library_name, function ) do {\
        ___internal_##function = (___internal_##function##FN*)platform->library.load_function( library_name, #function );\
        if( !___internal_##function ) {\
            fatal_log( "Failed to load '" #function "' from '" #library_name "'!" );\
            return false;\
        }\
    } while(0)

    AUDIO_LOAD_FUNCTION( ole32_lib, CoInitialize );
    AUDIO_LOAD_FUNCTION( ole32_lib, CoUninitialize );
    AUDIO_LOAD_FUNCTION( ole32_lib, CoCreateInstance );
    AUDIO_LOAD_FUNCTION( ole32_lib, CoTaskMemFree );

    platform->library.close( ole32_lib );

    #define AUDIO_CHECK_ERROR( function ) do{\
        if( FAILED( result ) ) {\
            fatal_log( #function " failed! {u,x}", result );\
            return false;\
        }\
    } while(0)

    HRESULT result = 0;

    /// Initialize COM.

    result = CoInitialize( NULL );
    AUDIO_CHECK_ERROR( CoInitialize );

    result = CoCreateInstance(
        &CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, &IID_IMMDeviceEnumerator,
        (void**)&global_device_enumerator );
    AUDIO_CHECK_ERROR( CoCreateInstance );

    /// Get default device.

    result = global_device_enumerator->lpVtbl->GetDefaultAudioEndpoint(
        global_device_enumerator, eRender, eConsole, &global_device );
    AUDIO_CHECK_ERROR( global_device_enumerator->GetDefaultAudioEndpoint );

    /// Activate audio client on device.

    result = global_device->lpVtbl->Activate(
        global_device, &IID_IAudioClient,
        CLSCTX_ALL, NULL, (void**)&global_audio_client );
    AUDIO_CHECK_ERROR( global_device->Activate );

    /// Define audio buffer format.

    global_format.nChannels       = 2;
    global_format.wFormatTag      = WAVE_FORMAT_PCM;
    global_format.nSamplesPerSec  = 44100;
    global_format.wBitsPerSample  = 16;
    global_format.nBlockAlign     =
        ( global_format.nChannels * global_format.wBitsPerSample ) / 8;
    global_format.nAvgBytesPerSec =
        global_format.nSamplesPerSec * global_format.nBlockAlign;
    global_format.cbSize = 0;

#if defined(LD_LOGGING)

    const char* wave_format_cstr =
        ___audio_wave_format_to_cstr( global_format.wFormatTag );

    info_log( "Audio -> Default wave format: " );
    info_log( "Audio -> nChannels:      {u}", global_format.nChannels );
    info_log( "Audio -> wFormatTag:     {cc}", wave_format_cstr );
    info_log( "Audio -> nSamplesPerSec: {u}", global_format.nSamplesPerSec );
    info_log( "Audio -> wBitsPerSample: {u}", global_format.wBitsPerSample );

#endif /* Logging enabled. */

    /// Specify buffer length in milliseconds.

    u64 buffer_length_ms = 500;
    REFTIME buffer_length_reftime = buffer_length_ms * REFTIMES_PER_MS;

    info_log( "Audio -> Audio buffer length ms: {u64}", buffer_length_ms );

    /// Create audio buffer.

    // NOTE(alicia): eventually I might implement resampling but for
    // now, just let wasapi resample the buffer for me
    DWORD initialize_stream_flags =
        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
        AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
    result = global_audio_client->lpVtbl->Initialize(
        global_audio_client,
        AUDCLNT_SHAREMODE_SHARED, initialize_stream_flags,
        buffer_length_reftime, 0,
        &global_format, NULL );
    AUDIO_CHECK_ERROR( global_audio_client->Initialize );

    /// Calculate actual audio buffer size.

    result = global_audio_client->lpVtbl->GetBufferSize(
        global_audio_client, &global_buffer_frame_count );
    AUDIO_CHECK_ERROR( global_audio_client->GetBufferSize );

    global_buffer_size =
        global_buffer_frame_count * (global_format.nChannels * ( global_format.wBitsPerSample / 8 ));

    info_log( "Audio -> Audio buffer size: {f,.2,b}", (f64)global_buffer_size );

    /// Get render client.
    
    result = global_audio_client->lpVtbl->GetService(
        global_audio_client, &IID_IAudioRenderClient,
        (void**)&global_audio_render_client );
    AUDIO_CHECK_ERROR( global_audio_client->GetService );

    /// Clear audio buffer.

    BYTE* audio_buffer = NULL;
    result = global_audio_render_client->lpVtbl->GetBuffer(
        global_audio_render_client, global_buffer_frame_count, &audio_buffer );
    AUDIO_CHECK_ERROR( global_audio_render_client->GetBuffer );

    memory_zero( audio_buffer, global_buffer_size );

    result = global_audio_render_client->lpVtbl->ReleaseBuffer(
        global_audio_render_client, global_buffer_frame_count, 0 );
    AUDIO_CHECK_ERROR( global_audio_render_client->ReleaseBuffer );

    /// Start playing audio buffer.

    result = global_audio_client->lpVtbl->Start( global_audio_client );
    AUDIO_CHECK_ERROR( global_audio_client->Start );

    info_log( "Audio subsystem successfully initialized." );
    return true;
}

global f32 t_sine = 0.0f;
#define AUDIO_VOLUME 3000
void audio_subsystem_fill_buffer(void) {
    UINT32 frames_available = 0;
    usize  bytes_available  = 0;
    BYTE*  buffer = NULL;
    if( ___audio_lock_buffer( &frames_available, &bytes_available, &buffer ) ) {

        i32 wave_period = global_format.nSamplesPerSec / 256;
        i16* sample_out = (i16*)buffer;

        f32 volume = global_master_volume * (f32)I16_MAX;

        for( usize i = 0; i < frames_available; ++i ) {
            f32 sine_value = sine( t_sine );

            i16 sample_value = (i16)( sine_value * volume );

            *sample_out++ = sample_value;
            *sample_out++ = sample_value;

            t_sine += 2.0f * F32_TAU / (f32)wave_period;
        }

        ___audio_unlock_buffer( frames_available );
    }
}

void audio_subsystem_shutdown(void) {
    #define COM_RELEASE( punk ) do {\
        if( (punk) != NULL ) {\
            (punk)->lpVtbl->Release( (punk) );\
            (punk) = NULL;\
        }\
    } while(0)

    global_audio_client->lpVtbl->Stop( global_audio_client );

    COM_RELEASE( global_device_enumerator );
    COM_RELEASE( global_device );
    COM_RELEASE( global_audio_client );
    COM_RELEASE( global_audio_render_client );

    CoUninitialize();

    info_log( "Audio subsystem shutdown." );
}

internal
const char* ___audio_wave_format_to_cstr( DWORD format_tag ) {
    switch( format_tag ) {
        case WAVE_FORMAT_PCM:        return "WAVE_FORMAT_PCM";
        case WAVE_FORMAT_EXTENSIBLE: return "WAVE_FORMAT_EXTENSIBLE";
        case WAVE_FORMAT_IEEE_FLOAT: return "WAVE_FORMAT_IEEE_FLOAT";
        case WAVE_FORMAT_DRM:        return "WAVE_FORMAT_DRM";
        case WAVE_FORMAT_ALAW:       return "WAVE_FORMAT_ALAW";
        case WAVE_FORMAT_MULAW:      return "WAVE_FORMAT_MULAW";
        case WAVE_FORMAT_ADPCM:      return "WAVE_FORMAT_ADPCM";
        default:                     return "UNKNOWN";
    }
}


