# Copyright 2019 The Cobalt Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This file was initially generated by starboard/tools/create_derived_build.py,
# though it may have been modified since its creation.

{
  'includes': [
    '../shared/starboard_platform.gypi',
  ],

  'variables': {
    'blittergles_starboard_platform_dependencies': [
      '<@(starboard_platform_dependencies)'
    ],

    # Exclude this dependency because our 'gl_type' is set to 'none'.
    'blittergles_starboard_platform_dependencies!': [
      '<(DEPTH)/starboard/egl_and_gles/egl_and_gles.gyp:egl_and_gles',
    ],
  },

  'targets': [
    {
      'target_name': 'starboard_platform',
      'type': 'static_library',
      'sources': [
        '<@(starboard_platform_sources)',
        '<(DEPTH)/starboard/shared/blittergles/blit_shader_program.cc',
        '<(DEPTH)/starboard/shared/blittergles/blit_shader_program.h',
        '<(DEPTH)/starboard/shared/blittergles/blitter_blit_rect_to_rect.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_context.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_context.h',
        '<(DEPTH)/starboard/shared/blittergles/blitter_create_context.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_create_default_device.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_create_pixel_data.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_create_render_target_surface.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_create_surface_from_pixel_data.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_create_swap_chain_from_window.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_destroy_context.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_destroy_device.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_destroy_pixel_data.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_destroy_surface.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_destroy_swap_chain.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_download_surface_pixels.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_fill_rect.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_flip_swap_chain.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_flush_context.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_get_max_contexts.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_get_pixel_data_pitch_in_bytes.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_get_pixel_data_pointer.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_get_render_target_from_surface.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_get_render_target_from_swap_chain.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_get_surface_info.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_internal.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_internal.h',
        '<(DEPTH)/starboard/shared/blittergles/blitter_is_pixel_format_supported_by_download_surface_pixels.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_is_pixel_format_supported_by_pixel_data.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_is_surface_format_supported_by_render_target_surface.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_set_blending.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_set_color.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_set_modulate_blits_with_color.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_set_render_target.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_set_scissor.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_surface.cc',
        '<(DEPTH)/starboard/shared/blittergles/blitter_surface.h',
        '<(DEPTH)/starboard/shared/blittergles/color_shader_program.cc',
        '<(DEPTH)/starboard/shared/blittergles/color_shader_program.h',
        '<(DEPTH)/starboard/shared/blittergles/shader_program.cc',
        '<(DEPTH)/starboard/shared/blittergles/shader_program.h',
        '<(DEPTH)/starboard/shared/starboard/blitter_blit_rect_to_rect_tiled.cc',
        '<(DEPTH)/starboard/shared/starboard/blitter_blit_rects_to_rects.cc',
      ],
      'defines': [
        # This must be defined when building Starboard, and must not when
        # building Starboard client code.
        'STARBOARD_IMPLEMENTATION',
      ],
      'dependencies': [
        '<@(blittergles_starboard_platform_dependencies)',
        '<(DEPTH)/starboard/egl_and_gles/egl_and_gles_angle.gyp:egl_and_gles_implementation',
      ],
    },
  ],
}
