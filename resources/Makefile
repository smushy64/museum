# * Description:  Makefile for Resources
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: July 22, 2023

RESOURCES_PATH := ../$(BUILD_PATH)/resources
SHADERS_PATH   := ../$(BUILD_PATH)/resources/shaders
SHIP_PATH      := $(RESOURCES_PATH)/ship.bmp
ICON_PATH      := ../$(BUILD_PATH)/icon.ico
ASTEROID_PATH  := $(RESOURCES_PATH)/asteroid.bmp
TEST_FONT_PATH := $(RESOURCES_PATH)/test.ttf

all: $(RESOURCES_PATH) $(SHADERS_PATH) $(TEST_FONT_PATH) $(ICON_PATH) $(ASTEROID_PATH) $(SHIP_PATH)

clean:
	@echo "Make: removing resources from build directory . . ."
	@rm -r -f $(SHADERS_PATH)
	@rm $(SHIP_PATH)
	@rm $(ICON_PATH)

spit:
	@echo $(SHADERS_PATH)

$(RESOURCES_PATH):
	@mkdir -p $(RESOURCES_PATH)

$(SHADERS_PATH):
	@echo "Make: copying shaders . . ."
	@cp ./shaders $(SHADERS_PATH) -r

$(ICON_PATH):icon.ico
	@echo "Make: copying program icon . . ."
	@cp icon.ico $(ICON_PATH)

$(SHIP_PATH):images/ship.bmp
	@echo "Make: copying ship.bmp . . ."
	@cp images/ship.bmp $(SHIP_PATH)

$(ASTEROID_PATH):images/asteroid.bmp
	@echo "Make: copying asteroid.bmp . . ."
	@cp images/asteroid.bmp $(ASTEROID_PATH)

$(TEST_FONT_PATH):test.ttf
	@echo "Make: copying test.ttf . . ."
	@cp test.ttf $(TEST_FONT_PATH)

.PHONY: all spit clean

