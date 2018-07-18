/*
 * Copyright 2018 Colomban Wendling <colomban@geany.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <glib.h>
#include <geanyplugin.h>
#include <Scintilla.h>

static const struct {
  const char       *pattern;  /**< Scintilla-style regex */
  GeanyFiletypeID   filetype; /**< Target filetype for this pattern */
} G_mappings[] = {
  { "^[ \\t]*#[ \\t]*include",  GEANY_FILETYPES_OBJECTIVEC },
  { "^[ \\t]*#[ \\t]*import",   GEANY_FILETYPES_OBJECTIVEC },
};

static void
on_document_open (GObject        *obj,
                  GeanyDocument  *doc,
                  gpointer        dummy)
{
  /* FIXME: don't change filetype if the file was part of the session, because
   * then the user might have made a manual choice we shouldn't override. */

  if (doc->file_type->id == GEANY_FILETYPES_MATLAB &&
      g_str_has_suffix (DOC_FILENAME (doc), ".m")) {
    ScintillaObject      *sci         = doc->editor->sci;
    gint                  line_count  = sci_get_line_count (sci);
    struct Sci_TextToFind ttf;

    /* don't check more than a hundred lines for performance reasons, and
     * because we ought to have made a decision by then anyway */
    ttf.chrg.cpMin = 0;
    ttf.chrg.cpMax = sci_get_line_end_position (sci, MIN (line_count, 100));

    for (guint i = 0; i < G_N_ELEMENTS (G_mappings); i++) {
      ttf.lpstrText = G_mappings[i].pattern;

      if (sci_find_text (sci, SCFIND_REGEXP, &ttf) >= 0) {
        GeanyFiletype *ft = filetypes_index (G_mappings[i].filetype);

        g_log (G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
               _("%s: %s (matched pattern \"%s\")"),
               DOC_FILENAME (doc), ft->name, G_mappings[i].pattern);
        document_set_filetype (doc, ft);
        break;
      }
    }
  }
}

static gboolean
init (GeanyPlugin *plugin,
      gpointer     data)
{
  plugin_signal_connect (plugin, NULL, "document-open", TRUE,
                         G_CALLBACK (on_document_open), NULL);

  return TRUE;
}

static void
cleanup (GeanyPlugin *plugin,
         gpointer     data)
{
}

G_MODULE_EXPORT
void geany_load_module (GeanyPlugin *plugin)
{
  main_locale_init (LOCALEDIR, GETTEXT_PACKAGE);

  plugin->info->name = _("Differentiate \".m\" between Matlab and Objective-C");
  plugin->info->description = _("Try and select the right filetype for .m files");
  plugin->info->version = "0.1";
  plugin->info->author = "Colomban Wendling <colomban@geany.org>";

  plugin->funcs->init = init;
  plugin->funcs->cleanup = cleanup;

  GEANY_PLUGIN_REGISTER (plugin, 225);
}
