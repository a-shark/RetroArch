/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2013-2014 - Jason Fetters
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../boolean.h"
#include "joypad_connection.h"

struct hidpad_ps3_data
{
   struct pad_connection* connection;
   send_control_t send_control;
   uint8_t data[512];
   uint32_t slot;
   uint32_t button_result;
   bool have_led;
   uint16_t motors[2];
};

static void hidpad_ps3_send_control(struct hidpad_ps3_data* device)
{
   /* TODO: Can this be modified to turn off motion tracking? */
   static uint8_t report_buffer[] = {
      0x52, 0x01,
      0x00, 0xFF, 0x00, 0xFF, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00,
      0xff, 0x27, 0x10, 0x00, 0x32,
      0xff, 0x27, 0x10, 0x00, 0x32,
      0xff, 0x27, 0x10, 0x00, 0x32,
      0xff, 0x27, 0x10, 0x00, 0x32,
      0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };
   
   report_buffer[11] = 1 << ((device->slot % 4) + 1);
   report_buffer[4] = device->motors[1] >> 8;
   report_buffer[6] = device->motors[0] >> 8;
    
   device->send_control(device->connection, report_buffer, sizeof(report_buffer));
}

static void* hidpad_ps3_connect(void *connect_data, uint32_t slot, send_control_t ptr)
{
   struct pad_connection* connection = (struct pad_connection*)connect_data;
   struct hidpad_ps3_data* device = (struct hidpad_ps3_data*)
    calloc(1, sizeof(struct hidpad_ps3_data));

   if (!device || !connection)
      return NULL;

   device->connection = connection;  
   device->slot = slot;
   device->send_control = ptr;
   
#ifdef IOS
   /* Magic packet to start reports. */
   static uint8_t data[] = {0x53, 0xF4, 0x42, 0x03, 0x00, 0x00};
   device->send_control(device->connection, data, 6);
#endif

   /* Without this, the digital buttons won't be reported. */
   hidpad_ps3_send_control(device);

   return device;
}

static void hidpad_ps3_disconnect(void *data)
{
   struct hidpad_ps3_data *device = (struct hidpad_ps3_data*)data;
    
   if (device)
      free(device);
}

static uint32_t hidpad_ps3_get_buttons(void *data)
{
   struct hidpad_ps3_data *device = (struct hidpad_ps3_data*)data;
   if (device)
      return device->button_result;
   return 0;
}

static int16_t hidpad_ps3_get_axis(void *data, unsigned axis)
{
   struct hidpad_ps3_data *device = (struct hidpad_ps3_data*)data;
    
   if (device && (axis < 4))
   {
      int val = device->data[7 + axis];
      val = (val << 8) - 0x8000;
      return (abs(val) > 0x1000) ? val : 0;
   }

   return 0;
}

static void hidpad_ps3_packet_handler(void *data, uint8_t *packet, uint16_t size)
{
   uint32_t i, pressed_keys;
   static const uint32_t button_mapping[17] =
   {
      RETRO_DEVICE_ID_JOYPAD_SELECT,
      RETRO_DEVICE_ID_JOYPAD_L3,
      RETRO_DEVICE_ID_JOYPAD_R3,
      RETRO_DEVICE_ID_JOYPAD_START,
      RETRO_DEVICE_ID_JOYPAD_UP,
      RETRO_DEVICE_ID_JOYPAD_RIGHT,
      RETRO_DEVICE_ID_JOYPAD_DOWN,
      RETRO_DEVICE_ID_JOYPAD_LEFT,
      RETRO_DEVICE_ID_JOYPAD_L2,
      RETRO_DEVICE_ID_JOYPAD_R2,
      RETRO_DEVICE_ID_JOYPAD_L,
      RETRO_DEVICE_ID_JOYPAD_R,
      RETRO_DEVICE_ID_JOYPAD_X,
      RETRO_DEVICE_ID_JOYPAD_A,
      RETRO_DEVICE_ID_JOYPAD_B,
      RETRO_DEVICE_ID_JOYPAD_Y,
      16 //< PS Button
   };
   struct hidpad_ps3_data *device = (struct hidpad_ps3_data*)data;
    
   if (!device)
      return;
    
   if (!device->have_led)
   {
      hidpad_ps3_send_control(device);
      device->have_led = true;
   }

   memcpy(device->data, packet, size);

   device->button_result = 0;

   pressed_keys = device->data[3] | (device->data[4] << 8) |
    ((device->data[5] & 1) << 16);

   for (i = 0; i < 17; i ++)
      device->button_result |= (pressed_keys & (1 << i)) ?
       (1 << button_mapping[i]) : 0;
}

static void hidpad_ps3_set_rumble(void *data,
   enum retro_rumble_effect effect, uint16_t strength)
{
   struct hidpad_ps3_data *device = (struct hidpad_ps3_data*)data;
   unsigned index = (effect == RETRO_RUMBLE_STRONG) ? 0 : 1;

   if (device && (device->motors[index] != strength))
   {
      device->motors[index] = strength;
      hidpad_ps3_send_control(device);
   }
}

pad_connection_interface_t pad_connection_ps3 = {
   hidpad_ps3_connect,
   hidpad_ps3_disconnect,
   hidpad_ps3_packet_handler,
   hidpad_ps3_set_rumble,
   hidpad_ps3_get_buttons,
   hidpad_ps3_get_axis,
};
