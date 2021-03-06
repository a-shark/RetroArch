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

#include "settings_list.h"

void settings_info_list_free(rarch_setting_info_t *list_info)
{
   if (list_info)
      free(list_info);
   list_info = NULL;
}

rarch_setting_info_t *settings_info_list_new(void)
{
   rarch_setting_info_t *list_info = (rarch_setting_info_t*)
      calloc(1, sizeof(*list_info));

   if (!list_info)
      return NULL;

   list_info->index = 0;
   list_info->size  = 32;

   return list_info;
}

bool settings_list_append(rarch_setting_t **list,
      rarch_setting_info_t *list_info, rarch_setting_t value)
{
   if (!list || !*list || !list_info)
      return false;

   if (list_info->index == list_info->size)
   {
      list_info->size *= 2;
      *list = (rarch_setting_t*)
         realloc(*list, sizeof(rarch_setting_t) * list_info->size);
      if (!*list)
         return false;
   }

   (*list)[list_info->index++] = value;
   return true;
}

void settings_list_current_add_flags(
      rarch_setting_t **list,
      rarch_setting_info_t *list_info,
      unsigned values)
{
   (*list)[list_info->index - 1].flags |= values;
}

void settings_list_current_add_range(
      rarch_setting_t **list,
      rarch_setting_info_t *list_info,
      float min, float max, float step,
      bool enforce_minrange_enable, bool enforce_maxrange_enable)
{
   (*list)[list_info->index - 1].min               = min;
   (*list)[list_info->index - 1].step              = step;
   (*list)[list_info->index - 1].max               = max;
   (*list)[list_info->index - 1].enforce_minrange  = enforce_minrange_enable;
   (*list)[list_info->index - 1].enforce_maxrange  = enforce_maxrange_enable;

   settings_list_current_add_flags(list, list_info, SD_FLAG_HAS_RANGE);
}

void settings_list_current_add_values(
      rarch_setting_t **list,
      rarch_setting_info_t *list_info,
      const char *values)
{
   (*list)[list_info->index -1].values = values;
}

void settings_list_current_add_cmd(
      rarch_setting_t **list,
      rarch_setting_info_t *list_info,
      unsigned values)
{
   (*list)[list_info->index -1].cmd_trigger.idx = values;
}

void settings_list_free(rarch_setting_t *list)
{
   if (list)
      free(list);
   list = NULL;
}

rarch_setting_t *settings_list_new(unsigned size)
{
   rarch_setting_t *list = (rarch_setting_t*)calloc(size, sizeof(*list));
   if (!list)
      return NULL;

   return list;
}
