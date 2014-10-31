/* See LICENSE file for license and copyright information */

#include <check.h>
#include <stdio.h>
#include <glib/gstdio.h>

#include <libzathura/plugin-manager.h>
#include <libzathura/plugin-api.h>

#include "plugin.h"
#include "utils.h"

zathura_document_t* document;
zathura_plugin_manager_t* plugin_manager;

static void setup_document(void) {
  fail_unless(zathura_plugin_manager_new(&plugin_manager) == ZATHURA_ERROR_OK);
  fail_unless(plugin_manager != NULL);
  fail_unless(zathura_plugin_manager_load(plugin_manager, get_plugin_path()) == ZATHURA_ERROR_OK);

  zathura_plugin_t* plugin = NULL;
  fail_unless(zathura_plugin_manager_get_plugin(plugin_manager, &plugin, "application/pdf") == ZATHURA_ERROR_OK);
  fail_unless(plugin != NULL);

  fail_unless(zathura_plugin_open_document(plugin, &document, "files/empty.pdf", NULL) == ZATHURA_ERROR_OK);
  fail_unless(document != NULL);
}

static void teardown_document(void) {
  fail_unless(zathura_document_free(document) == ZATHURA_ERROR_OK);
  document = NULL;

  fail_unless(zathura_plugin_manager_free(plugin_manager) == ZATHURA_ERROR_OK);
  plugin_manager = NULL;
}

START_TEST(test_pdf_document_open) {
  /* basic invalid arguments */
  fail_unless(pdf_document_open(NULL) == ZATHURA_ERROR_INVALID_ARGUMENTS);
} END_TEST

START_TEST(test_pdf_document_free) {
  /* basic invalid arguments */
  fail_unless(pdf_document_free(NULL) == ZATHURA_ERROR_INVALID_ARGUMENTS);
} END_TEST

START_TEST(test_pdf_document_save_as) {
  /* basic invalid arguments */
  fail_unless(pdf_document_save_as(NULL, NULL)     == ZATHURA_ERROR_INVALID_ARGUMENTS);
  fail_unless(pdf_document_save_as(document, NULL) == ZATHURA_ERROR_INVALID_ARGUMENTS);
  fail_unless(pdf_document_save_as(document, "")   == ZATHURA_ERROR_INVALID_ARGUMENTS);

  /* valid arguments */
  char* path;
  gint file_handle;
  if ((file_handle = g_file_open_tmp(NULL, &path, NULL)) != -1) {
    fail_unless(pdf_document_save_as(document, path) == ZATHURA_ERROR_OK);
    close(file_handle);

    g_remove(path);
    g_free(path);
  }
} END_TEST

Suite*
suite_document(void)
{
  TCase* tcase = NULL;
  Suite* suite = suite_create("document");

  tcase = tcase_create("basic");
  tcase_add_checked_fixture(tcase, setup_document, teardown_document);
  tcase_add_test(tcase, test_pdf_document_open);
  tcase_add_test(tcase, test_pdf_document_free);
  suite_add_tcase(suite, tcase);

  tcase = tcase_create("save-as");
  tcase_add_checked_fixture(tcase, setup_document, teardown_document);
  tcase_add_test(tcase, test_pdf_document_save_as);
  suite_add_tcase(suite, tcase);

  return suite;
}
