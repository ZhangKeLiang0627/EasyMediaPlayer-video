CSRCS += lv_draw_g2d.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sunxi_g2d
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sunxi_g2d

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sunxi_g2d"
