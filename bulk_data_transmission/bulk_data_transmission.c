// Bulk data transmission example for Crosswalk
// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "public\XW_Extension.h"
#include "public\XW_Extension_SyncMessage.h"
#include "Windows.h"
#undef PostMessage

static const char* kSource_bulk_data_api = "var bulkDataListener = null;"
""
"extension.setMessageListener(function(msg) {"
"  if (bulkDataListener instanceof Function) {"
"    bulkDataListener(msg);"
"  };"
"});"
""
"exports.requestBulkDataAsync = function(power, callback) {"
"  bulkDataListener = callback;"
"  extension.postMessage(power.toString());"
"};"
""
"exports.requestBulkDataSync = function(power) {"
"  return extension.internal.sendSyncMessage(power.toString());"
"};"
;

static XW_Extension g_extension = 0;
static const XW_CoreInterface* g_core = NULL;
static const XW_MessagingInterface* g_messaging = NULL;
static const XW_Internal_SyncMessagingInterface* g_sync_messaging = NULL;

static void instance_created(XW_Instance instance) {
  printf("Instance %d created!\n", instance);
}

static void instance_destroyed(XW_Instance instance) {
  printf("Instance %d destroyed!\n", instance);
}

static void handle_message(XW_Instance instance, const char* message) {
  int size = atoi(message);
  char* data = malloc(size + 1);
  SYSTEMTIME time;
  int time_ms, i = 0; 
  char t[20];
  GetLocalTime(&time);
  time_ms = time.wMilliseconds + (time.wSecond + (time.wMinute + time.wHour * 60) * 60) * 1000;
  _itoa_s(time_ms, t, 20, 10);
  memset(data, 'p', size);
  while (t[i] != 0)
  {
	  data[i] = t[i];
	  i++;
  }
  data[size] = '\0';
  printf("Instance %d created %d bytes of data chunk from native.\n", instance, size);
  g_messaging->PostMessage(instance, data);
  free(data);
}

void handle_sync_message(XW_Instance instance, const char* message) {
	int size = atoi(message);
	char* data = malloc(size + 1);
	SYSTEMTIME time;
	int time_ms, i = 0;
	char t[20];
	GetLocalTime(&time);
	time_ms = time.wMilliseconds + (time.wSecond + (time.wMinute + time.wHour * 60) * 60) * 1000;
	_itoa_s(time_ms, t, 20, 10);
	memset(data, 'p', size);
	while (t[i] != 0)
	{
		data[i] = t[i];
		i++;
	}
	data[size] = '\0';
	printf("Instance %d created %d bytes of data chunk from native.\n", instance, size);
	g_sync_messaging->SetSyncReply(instance, data);
	free(data);
}

static void XW_shutdown(XW_Extension extension) {
  printf("Shutdown\n");
}

// this is the only function which needs to be public
int32_t XW_Initialize(XW_Extension extension, XW_GetInterface get_interface) {
  // set up the extension
  g_extension = extension;
  g_core = get_interface(XW_CORE_INTERFACE);
  g_core->SetExtensionName(extension, "bulkData");

  g_core->SetJavaScriptAPI(extension, kSource_bulk_data_api);

  g_core->RegisterInstanceCallbacks(
    extension, instance_created, instance_destroyed);
  g_core->RegisterShutdownCallback(extension, XW_shutdown);

  g_messaging = get_interface(XW_MESSAGING_INTERFACE);
  g_messaging->Register(extension, handle_message);

  g_sync_messaging = get_interface(XW_INTERNAL_SYNC_MESSAGING_INTERFACE);
  g_sync_messaging->Register(extension, handle_sync_message);

  return XW_OK;
}
