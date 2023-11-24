#ifndef D_A_OBJ_BARREL2_H
#define D_A_OBJ_BARREL2_H

#include "f_op/f_op_actor.h"
#include "d/d_cc_d.h"

namespace daObjBarrel2 {
    class Act_c : public fopAc_ac_c {
    public:
        void delete_req() {}
        void exit_req() {}
        void get_item_id() {}
        void make_coming(cXyz*, int, daObjBarrel2::Type_e, int, bool, short, daObjBuoyflag::Texture_e) {}
        void make_prm(daObjBarrel2::Type_e, int, bool, bool, daObjBuoyflag::Texture_e) {}
    
        void solidHeapCB(fopAc_ac_c*);
        void create_heap();
        void _create();
        void _delete();
        void tg_hitCB(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);
        void co_hitCB(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);
        void cull_set_draw();
        void cull_set_move();
        void demo_mode_chk() const;
        void mode_afl_init();
        void mode_afl();
        void mode_exit_v_init();
        void mode_exit_v();
        void mode_exit_h_init();
        void mode_exit_h();
        void mode_exit_mine_init();
        void mode_exit_mine();
        void mode_explode_init();
        void mode_explode();
        void mode_demo_break0_init();
        void mode_demo_break0();
        void mode_demo_break1_init();
        void mode_demo_break1();
        void mode_demo_explode0_init();
        void mode_demo_explode0();
        void mode_demo_explode1_init();
        void mode_demo_explode1();
        void mode_proc_call();
        void set_pos_y();
        void set_mtx();
        void init_mtx();
        void eff_break();
        void eff_explode();
        void set_item_position();
        void item_drop_init(float);
        void item_drop();
        void item_delete();
        void item_give();
        void item_connect_check();
        void buoy_jump(float);
        void set_water_pos();
        void afl_sway();
        void mine_chk_range_flash();
        void mine_chk_range_explode();
        void mine_chk_range_damage();
        void execute_sub();
        void _execute();
        void _draw();
    
    public:
        /* Place member variables here */
    };
};

#endif /* D_A_OBJ_BARREL2_H */