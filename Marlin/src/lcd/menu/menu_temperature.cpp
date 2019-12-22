/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// Temperature Menu
//

#include "../../inc/MarlinConfigPre.h"

#if HAS_LCD_MENU

#include "menu.h"
#include "../../module/temperature.h"

#if FAN_COUNT > 1 || ENABLED(SINGLENOZZLE)
  #include "../../module/motion.h"
#endif

#if ENABLED(SINGLENOZZLE)
  #include "../../module/tool_change.h"
#endif

// Initialized by settings.load()
int16_t MarlinUI::preheat_hotend_temp[PREHEAT_MATERIAL_COUNT], 
        MarlinUI::preheat_bed_temp[PREHEAT_MATERIAL_COUNT];
uint8_t MarlinUI::preheat_fan_speed[PREHEAT_MATERIAL_COUNT];

//
// "Temperature" submenu items
//

void _lcd_preheat(const int16_t endnum, const int16_t temph, const int16_t tempb, const uint8_t fan) {
  #if HOTENDS
    if (temph > 0) thermalManager.setTargetHotend(_MIN(heater_maxtemp[endnum] - 15, temph), endnum);
  #endif
  #if HAS_HEATED_BED
    if (tempb >= 0) thermalManager.setTargetBed(tempb);
  #else
    UNUSED(tempb);
  #endif
  #if FAN_COUNT > 0
    #if FAN_COUNT > 1
      thermalManager.set_fan_speed(active_extruder < FAN_COUNT ? active_extruder : 0, fan);
    #else
      thermalManager.set_fan_speed(0, fan);
    #endif
  #else
    UNUSED(fan);
  #endif
  ui.return_to_status();
}

#if HAS_TEMP_HOTEND
  inline void _preheat_end(const uint8_t m, const uint8_t e) {
    _lcd_preheat(e, ui.preheat_hotend_temp[m], -1, ui.preheat_fan_speed[m]);
  }
  #if HAS_HEATED_BED
    inline void _preheat_both(const uint8_t m, const uint8_t e) {
      _lcd_preheat(e, ui.preheat_hotend_temp[m], ui.preheat_bed_temp[m], ui.preheat_fan_speed[m]);
    }
  #endif
#endif
#if HAS_HEATED_BED
  inline void _preheat_bed(const uint8_t m) {
    _lcd_preheat(0, 0, ui.preheat_bed_temp[m], ui.preheat_fan_speed[m]);
  }
#endif

#if HAS_TEMP_HOTEND || HAS_HEATED_BED

  #define _PREHEAT_ITEMS(M,N) do{ \
    ACTION_ITEM_N(N, MSG_PREHEAT_##M##_H, []{ _preheat_both(M-1, MenuItemBase::itemIndex); }); \
    ACTION_ITEM_N(N, MSG_PREHEAT_##M##_END_E, []{ _preheat_end(M-1, MenuItemBase::itemIndex); }); \
  }while(0)
  #if HAS_HEATED_BED
    #define PREHEAT_ITEMS(M,N) _PREHEAT_ITEMS(M,N)
  #else
    #define PREHEAT_ITEMS(M,N) \
      ACTION_ITEM_N(N, MSG_PREHEAT_##M##_H, []{ _preheat_end(M-1, MenuItemBase::itemIndex); })
  #endif

  #if PREHEAT_MATERIAL_COUNT > 0
    void menu_preheat_m1() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_1, []{ _preheat_both(0, 0); });
          ACTION_ITEM(MSG_PREHEAT_1_END, []{ _preheat_end(0, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_1, []{ _preheat_end(0, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(1,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(1,n);
          ACTION_ITEM(MSG_PREHEAT_1_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(0);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[0], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_1_BEDONLY, []{ _preheat_bed(0); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 1
    void menu_preheat_m2() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_2, []{ _preheat_both(1, 0); });
          ACTION_ITEM(MSG_PREHEAT_2_END, []{ _preheat_end(1, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_2, []{ _preheat_end(1, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(2,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(2,n);
          ACTION_ITEM(MSG_PREHEAT_2_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(1);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[1], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_2_BEDONLY, []{ _preheat_bed(1); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 2
    void menu_preheat_m3() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_3, []{ _preheat_both(2, 0); });
          ACTION_ITEM(MSG_PREHEAT_3_END, []{ _preheat_end(2, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_3, []{ _preheat_end(2, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(3,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(3,n);
          ACTION_ITEM(MSG_PREHEAT_3_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(2);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[2], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_3_BEDONLY, []{ _preheat_bed(2); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 3
    void menu_preheat_m4() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_4, []{ _preheat_both(3, 0); });
          ACTION_ITEM(MSG_PREHEAT_4_END, []{ _preheat_end(3, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_4, []{ _preheat_end(3, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(4,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(4,n);
          ACTION_ITEM(MSG_PREHEAT_4_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(3);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[3], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_4_BEDONLY, []{ _preheat_bed(3); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 4
    void menu_preheat_m5() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_5, []{ _preheat_both(4, 0); });
          ACTION_ITEM(MSG_PREHEAT_5_END, []{ _preheat_end(4, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_5, []{ _preheat_end(4, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(5,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(5,n);
          ACTION_ITEM(MSG_PREHEAT_5_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(4);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[4], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_5_BEDONLY, []{ _preheat_bed(4); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 5
    void menu_preheat_m6() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_6, []{ _preheat_both(5, 0); });
          ACTION_ITEM(MSG_PREHEAT_6_END, []{ _preheat_end(5, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_6, []{ _preheat_end(5, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(6,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(6,n);
          ACTION_ITEM(MSG_PREHEAT_6_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(5);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[5], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_6_BEDONLY, []{ _preheat_bed(5); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 6
    void menu_preheat_m7() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_7, []{ _preheat_both(6, 0); });
          ACTION_ITEM(MSG_PREHEAT_7_END, []{ _preheat_end(6, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_7, []{ _preheat_end(6, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(7,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(7,n);
          ACTION_ITEM(MSG_PREHEAT_7_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(6);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[6], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_7_BEDONLY, []{ _preheat_bed(6); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 7
    void menu_preheat_m8() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_8, []{ _preheat_both(7, 0); });
          ACTION_ITEM(MSG_PREHEAT_8_END, []{ _preheat_end(7, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_8, []{ _preheat_end(7, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(8,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(8,n);
          ACTION_ITEM(MSG_PREHEAT_8_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(7);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[7], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_8_BEDONLY, []{ _preheat_bed(7); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 8
    void menu_preheat_m9() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_9, []{ _preheat_both(8, 0); });
          ACTION_ITEM(MSG_PREHEAT_9_END, []{ _preheat_end(8, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_9, []{ _preheat_end(8, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(9,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(9,n);
          ACTION_ITEM(MSG_PREHEAT_9_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(8);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[8], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_9_BEDONLY, []{ _preheat_bed(8); });
      #endif
      END_MENU();
    }
  #endif

  #if PREHEAT_MATERIAL_COUNT > 9
    void menu_preheat_m10() {
      START_MENU();
      BACK_ITEM(MSG_TEMPERATURE);
      #if HOTENDS == 1
        #if HAS_HEATED_BED
          ACTION_ITEM(MSG_PREHEAT_10, []{ _preheat_both(9, 0); });
          ACTION_ITEM(MSG_PREHEAT_10_END, []{ _preheat_end(9, 0); });
        #else
          ACTION_ITEM(MSG_PREHEAT_10, []{ _preheat_end(9, 0); });
        #endif
      #elif HOTENDS > 1
        #if HAS_HEATED_BED
          _PREHEAT_ITEMS(10,0);
        #endif
        for (uint8_t n = 1; n < HOTENDS; n++) PREHEAT_ITEMS(10,n);
          ACTION_ITEM(MSG_PREHEAT_10_ALL, []() {
          #if HAS_HEATED_BED
            _preheat_bed(9);
          #endif
          HOTEND_LOOP() thermalManager.setTargetHotend(ui.preheat_hotend_temp[9], e);
        });
      #endif // HOTENDS > 1
      #if HAS_HEATED_BED
        ACTION_ITEM(MSG_PREHEAT_10_BEDONLY, []{ _preheat_bed(9); });
      #endif
      END_MENU();
    }
  #endif

  void lcd_cooldown() {
    thermalManager.zero_fan_speeds();
    thermalManager.disable_all_heaters();
    ui.return_to_status();
  }

#endif // HAS_TEMP_HOTEND || HAS_HEATED_BED

void menu_temperature() {
  START_MENU();
  BACK_ITEM(MSG_MAIN);

  //
  // Nozzle:
  // Nozzle [1-5]:
  //
  #if HOTENDS == 1
    EDIT_ITEM_FAST(int3, MSG_NOZZLE, &thermalManager.temp_hotend[0].target, 0, HEATER_0_MAXTEMP - 15, []{ thermalManager.start_watching_hotend(0); });
  #elif HOTENDS > 1
    #define EDIT_TARGET(N) EDIT_ITEM_FAST_N(int3, N, MSG_NOZZLE_N, &thermalManager.temp_hotend[N].target, 0, heater_maxtemp[N] - 15, []{ thermalManager.start_watching_hotend(MenuItemBase::itemIndex); })
    HOTEND_LOOP() EDIT_TARGET(e);
  #endif

  #if ENABLED(SINGLENOZZLE)
    EDIT_ITEM_FAST(uint16_3, MSG_NOZZLE_STANDBY, &singlenozzle_temp[active_extruder ? 0 : 1], 0, HEATER_0_MAXTEMP - 15);
  #endif

  //
  // Bed:
  //
  #if HAS_HEATED_BED
    EDIT_ITEM_FAST(int3, MSG_BED, &thermalManager.temp_bed.target, 0, BED_MAXTEMP - 10, thermalManager.start_watching_bed);
  #endif

  //
  // Chamber:
  //
  #if HAS_HEATED_CHAMBER
    EDIT_ITEM_FAST(int3, MSG_CHAMBER, &thermalManager.temp_chamber.target, 0, CHAMBER_MAXTEMP - 5, thermalManager.start_watching_chamber);
  #endif

  //
  // Fan Speed:
  //
  #if FAN_COUNT > 0
    #if HAS_FAN0
      editable.uint8 = thermalManager.fan_speed[0];
      EDIT_ITEM_FAST_N(percent, 1, MSG_FIRST_FAN_SPEED, &editable.uint8, 0, 255, []{ thermalManager.set_fan_speed(0, editable.uint8); });
      #if ENABLED(EXTRA_FAN_SPEED)
        EDIT_ITEM_FAST_N(percent, 1, MSG_FIRST_EXTRA_FAN_SPEED, &thermalManager.new_fan_speed[0], 3, 255);
      #endif
    #endif
    #if HAS_FAN1
      editable.uint8 = thermalManager.fan_speed[1];
      EDIT_ITEM_FAST_N(percent, 2, MSG_FAN_SPEED_N, &editable.uint8, 0, 255, []{ thermalManager.set_fan_speed(1, editable.uint8); });
      #if ENABLED(EXTRA_FAN_SPEED)
        EDIT_ITEM_FAST_N(percent, 2, MSG_EXTRA_FAN_SPEED_N, &thermalManager.new_fan_speed[1], 3, 255);
      #endif
    #elif ENABLED(SINGLENOZZLE) && EXTRUDERS > 1
      editable.uint8 = thermalManager.fan_speed[1];
      EDIT_ITEM_FAST_N(percent, 2, MSG_STORED_FAN_N, &editable.uint8, 0, 255, []{ thermalManager.set_fan_speed(1, editable.uint8); });
    #endif
    #if HAS_FAN2
      editable.uint8 = thermalManager.fan_speed[2];
      EDIT_ITEM_FAST_N(percent, 3, MSG_FAN_SPEED_N, &editable.uint8, 0, 255, []{ thermalManager.set_fan_speed(2, editable.uint8); });
      #if ENABLED(EXTRA_FAN_SPEED)
        EDIT_ITEM_FAST_N(percent, 3, MSG_EXTRA_FAN_SPEED_N, &thermalManager.new_fan_speed[2], 3, 255);
      #endif
    #elif ENABLED(SINGLENOZZLE) && EXTRUDERS > 2
      editable.uint8 = thermalManager.fan_speed[2];
      EDIT_ITEM_FAST_N(percent, 3, MSG_STORED_FAN_N, &editable.uint8, 0, 255, []{ thermalManager.set_fan_speed(2, editable.uint8); });
    #endif
  #endif // FAN_COUNT > 0

  #if HAS_TEMP_HOTEND

    //
    // Preheat for Configured Materials
    //
    #if TEMP_SENSOR_1 != 0 || TEMP_SENSOR_2 != 0 || TEMP_SENSOR_3 != 0 || TEMP_SENSOR_4 != 0 || TEMP_SENSOR_5 != 0 || HAS_HEATED_BED
      #if PREHEAT_MATERIAL_COUNT > 0
        SUBMENU(MSG_PREHEAT_1, menu_preheat_m1);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 1
        SUBMENU(MSG_PREHEAT_2, menu_preheat_m2);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 2
        SUBMENU(MSG_PREHEAT_3, menu_preheat_m3);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 3
        SUBMENU(MSG_PREHEAT_4, menu_preheat_m4);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 4
        SUBMENU(MSG_PREHEAT_5, menu_preheat_m5);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 5
        SUBMENU(MSG_PREHEAT_6, menu_preheat_m6);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 6
        SUBMENU(MSG_PREHEAT_7, menu_preheat_m7);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 7
        SUBMENU(MSG_PREHEAT_8, menu_preheat_m8);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 8
        SUBMENU(MSG_PREHEAT_9, menu_preheat_m9);
      #endif
      #if PREHEAT_MATERIAL_COUNT > 9
        SUBMENU(MSG_PREHEAT_10, menu_preheat_m10);
      #endif
    #else
      #if PREHEAT_MATERIAL_COUNT > 0
        ACTION_ITEM(MSG_PREHEAT_1, []{ _preheat_end(0, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 1
        ACTION_ITEM(MSG_PREHEAT_2, []{ _preheat_end(1, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 2
        ACTION_ITEM(MSG_PREHEAT_3, []{ _preheat_end(2, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 3
        ACTION_ITEM(MSG_PREHEAT_4, []{ _preheat_end(3, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 4
        ACTION_ITEM(MSG_PREHEAT_5, []{ _preheat_end(4, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 5
        ACTION_ITEM(MSG_PREHEAT_6, []{ _preheat_end(5, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 6
        ACTION_ITEM(MSG_PREHEAT_7, []{ _preheat_end(6, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 7
        ACTION_ITEM(MSG_PREHEAT_8, []{ _preheat_end(7, 0); });
      #endif
        #if PREHEAT_MATERIAL_COUNT > 8
        ACTION_ITEM(MSG_PREHEAT_9, []{ _preheat_end(8, 0); });
      #endif
      #if PREHEAT_MATERIAL_COUNT > 9
        ACTION_ITEM(MSG_PREHEAT_10, []{ _preheat_end(9, 0); });
      #endif
    #endif

    //
    // Cooldown
    //
    bool has_heat = false;
    HOTEND_LOOP() if (thermalManager.temp_hotend[HOTEND_INDEX].target) { has_heat = true; break; }
    #if HAS_TEMP_BED
      if (thermalManager.temp_bed.target) has_heat = true;
    #endif
    if (has_heat) ACTION_ITEM(MSG_COOLDOWN, lcd_cooldown);

  #endif // HAS_TEMP_HOTEND

  END_MENU();
}

#endif // HAS_LCD_MENU
