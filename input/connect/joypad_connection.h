/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2013-2014 - Jason Fetters
 *  Copyright (C) 2011-2014 - Daniel De Matteis
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

#ifndef _JOYPAD_CONNECTION_H
#define _JOYPAD_CONNECTION_H

#include <stdint.h>
#include <stddef.h>
#include "../input_common.h"

typedef void (*send_control_t)(void *data, uint8_t *buf, size_t size);

typedef struct pad_connection_interface
{
   void*    (*connect)(void *data, uint32_t slot, send_control_t ptr);
   void     (*disconnect)(void* device);
   void     (*packet_handler)(void* device, uint8_t *packet, uint16_t size);
   void     (*set_rumble)(void* device, enum retro_rumble_effect effect,
         uint16_t strength);
   uint32_t (*get_buttons)(void *data);
   int16_t  (*get_axis)(void *data, unsigned axis);
} pad_connection_interface_t;

extern pad_connection_interface_t pad_connection_wii;
extern pad_connection_interface_t pad_connection_ps3;

typedef struct
{
    bool used;
    struct pad_connection_interface *iface;
    void* data;
    
    bool is_gcapi;
} joypad_connection_t;

int32_t pad_connection_connect(joypad_connection_t *joyconn,
   const char* name, void *data, send_control_t ptr);

int32_t apple_joypad_connect_gcapi(joypad_connection_t *joyconn);

void *pad_connection_init(unsigned pads);

void pad_connection_destroy(joypad_connection_t *joyconn);

void pad_connection_disconnect(joypad_connection_t *joyconn,
   unsigned index);

void pad_connection_packet(joypad_connection_t *joyconn,
   unsigned index, uint8_t* data, uint32_t length);

uint32_t pad_connection_get_buttons(joypad_connection_t *joyconn,
   unsigned index);

int16_t pad_connection_get_axis(joypad_connection_t *joyconn,
   unsigned index, unsigned i);

/* Determine if connected joypad is a hidpad backed device.
 * If false, pad_connection_packet cannot be used */

bool pad_connection_has_interface(joypad_connection_t *joyconn,
   unsigned index);

#endif
