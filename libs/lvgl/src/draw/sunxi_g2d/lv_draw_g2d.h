/**
 * @file lv_draw_g2d.h
 *
 */

#ifndef LV_DRAW_G2D_H
#define LV_DRAW_G2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"
#include "../../hal/lv_hal_disp.h"
#include "../sw/lv_draw_sw.h"

#if USE_SUNXIFB_G2D

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_g2d_ctx_t;

struct _lv_disp_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_g2d_init_ctx(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_g2d_deinit_ctx(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_SUNXI_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_G2D_H*/
