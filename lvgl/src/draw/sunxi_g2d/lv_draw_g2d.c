/**
 * @file lv_draw_g2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d.h"
#include "../../core/lv_refr.h"

#if USE_SUNXIFB_G2D
#include "../../../../lv_drivers/display/sunxig2d.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_g2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

static void lv_draw_g2d_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                                                  const lv_area_t * coords, const uint8_t * src_buf, lv_img_cf_t cf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_init_ctx(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_g2d_ctx_t * g2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    g2d_draw_ctx->blend = lv_draw_g2d_blend;
    g2d_draw_ctx->base_draw.draw_img_decoded = lv_draw_g2d_img_decoded;
}

void lv_draw_g2d_deinit_ctx(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}


static void lv_draw_g2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    const lv_opa_t * mask;
    bool done = false;
    if(dsc->mask_buf == NULL) mask = NULL;
    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;

    lv_color_t * dest_buf = draw_ctx->buf;
    const lv_color_t * src_buf = dsc->src_buf;

    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL && mask == NULL) {
        if(dsc->src_buf == NULL) {
#if LV_USE_SUNXIFB_G2D_FILL
            if(dsc->opa < LV_OPA_MAX && lv_area_get_size(&blend_area) >=
                     sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_OPA_FILL)) {
                if(!sunxifb_g2d_fill(dest_buf, draw_ctx->buf_area, &blend_area, dsc->color, dsc->opa))
                    done = true;
            }
#endif
        }
        else {
#if LV_USE_SUNXIFB_G2D_BLIT
            if(lv_area_get_size(&blend_area) >= sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_BLIT)) {
                if(!sunxifb_g2d_blit(dest_buf, draw_ctx->buf_area, &blend_area,
                       (lv_color_t*) src_buf, dsc->blend_area, dsc->opa))
                    done = true;
            }
#endif
        }
    }

    if(!done)
       lv_draw_sw_blend_basic(draw_ctx, dsc);
}



static void lv_draw_g2d_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                                                  const lv_area_t * coords, const uint8_t * src_buf, lv_img_cf_t cf)
{
    /*Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, draw_ctx->clip_area);

    bool mask_any = lv_draw_mask_is_any(&draw_area);
    bool transform = draw_dsc->angle != 0 || draw_dsc->zoom != LV_IMG_ZOOM_NONE ? true : false;
    bool recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);
    bool scale = (draw_dsc->zoom != LV_IMG_ZOOM_NONE);

    lv_area_t blend_area;

    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
	if(!_lv_area_intersect(&blend_area, coords, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_color_t * dest_buf = draw_ctx->buf;

#if LV_USE_SUNXIFB_G2D_BLIT
    /*The simplest case just copy the pixels into the draw_buf*/
    if(!mask_any && !transform && cf == LV_IMG_CF_TRUE_COLOR && !recolor) {
        if(lv_area_get_size(&blend_area) >= sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_BLIT)) {
            if(!sunxifb_g2d_blit(dest_buf, draw_ctx->buf_area, &blend_area,
                    (lv_color_t*) src_buf, coords, draw_dsc->opa))
                return;
        }
    }
#endif

#if LV_USE_SUNXIFB_G2D_FILL
    if(!mask_any && !transform && !lv_img_cf_is_chroma_keyed(cf) && recolor) {
        if(draw_dsc->opa < LV_OPA_MAX && lv_area_get_size(&blend_area) >=
                sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_OPA_FILL)) {
            if(!sunxifb_g2d_fill(dest_buf, draw_ctx->buf_area, &blend_area,
                    draw_dsc->recolor, draw_dsc->opa))
                return;
        }
    }
#endif

#if LV_USE_SUNXIFB_G2D_BLEND
    /*chroma keyed*/
    if (!mask_any && !transform && lv_img_cf_is_chroma_keyed(cf) && !recolor) {
        if (lv_area_get_size(&blend_area) >= sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_BLEND)) {
            if (!sunxifb_g2d_blend(dest_buf, draw_ctx->buf_area, &blend_area,
                    (lv_color_t*) src_buf, coords, draw_dsc->opa, true))
                return;
        }
    }

    /*alpha blending*/
    if (!mask_any && !transform && lv_img_cf_has_alpha(cf) && !recolor) {
        if (lv_area_get_size(&blend_area) >= sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_BLEND)) {
            if (!sunxifb_g2d_blend(dest_buf, draw_ctx->buf_area, &blend_area,
                    (lv_color_t*) src_buf, coords, draw_dsc->opa, false))
                return;
        }
    }
#endif

#if LV_USE_SUNXIFB_G2D_SCALE
    lv_area_move(&draw_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
    if (!mask_any && draw_dsc->angle == 0 && scale && !lv_img_cf_is_chroma_keyed(cf) && !recolor) {
        if (lv_area_get_size(&blend_area) >= sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_SCALE)) {
            if (!sunxifb_g2d_scale(dest_buf, draw_ctx->buf_area,
                    &draw_area, (lv_color_t*) src_buf, coords,
                    draw_dsc->opa, draw_dsc->zoom, &draw_dsc->pivot))
                return;
        }
    }
#endif

    lv_draw_sw_img_decoded(draw_ctx, draw_dsc, coords, src_buf, cf);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif  /*LV_USE_SUNXI_G2D*/
