/**
 * ecutools: IoT Automotive Tuning, Diagnostics & Analytics
 * Copyright (C) 2014 Jeremy Hahn
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
 */

#ifndef PASSTHRUSHADOW_H_
#define PASSTHRUSHADOW_H_

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <errno.h>
#include "aws_iot_src/include/aws_iot_log.h"
#include "aws_iot_src/include/aws_iot_version.h"
#include "aws_iot_src/include/aws_iot_mqtt_client_interface.h"
#include "aws_iot_src/include/aws_iot_shadow_interface.h"
#include "aws_iot_config.h"

char DELTA_REPORT[SHADOW_MAX_SIZE_OF_RX_BUFFER];
bool messageArrivedOnDelta;

typedef struct _shadow_log {
  int *type;
  char *file;
} shadow_log;

typedef struct _shadow_reported {
  int *connection;
  shadow_log *log;
} shadow_report;

typedef struct _shadow_metadata {
  shadow_report *reported;
} shadow_metadata;

typedef struct _shadow_desired {
  char *connection;
} shadow_desired;

typedef struct _shadow_state {
  shadow_report *reported;
  shadow_desired *desired;
} shadow_state;

typedef struct _shadow_message {
  shadow_state *state;
  shadow_metadata *metadata;
  uint64_t version;
  uint64_t timestamp;
  char *clientToken;
} shadow_message;

typedef struct _passthru_shadow {
  char *clientId;
  IoT_Error_t rc;
  AWS_IoT_Client *mqttClient;
  pthread_t yield_thread;
  pthread_mutex_t shadow_update_lock;
  void (*onopen)(struct _passthru_shadow *);
  void (*ondelta)(const char *pJsonValueBuffer, uint32_t valueLength, jsonStruct_t *pJsonStruct_t);
  void (*onupdate)(const char *pThingName, ShadowActions_t action, Shadow_Ack_Status_t status, const char *pReceivedJsonDocument, void *pContextData);
  void (*onget)(const char *pJsonValueBuffer, uint32_t valueLength, jsonStruct_t *pJsonStruct_t);
  void (*ondisconnect)(void);
  void (*onerror)(struct _passthru_shadow *, const char *message);
} passthru_shadow;

int passthru_shadow_connect(passthru_shadow *shadow);
bool passthru_shadow_build_report_json(char *pJsonDocument, size_t maxSizeOfJsonDocument, const char *pReceivedDeltaData, uint32_t lengthDelta);
int passthru_shadow_report_delta(passthru_shadow *shadow);
void passthru_shadow_get(passthru_shadow *shadow);
int passthru_shadow_update(passthru_shadow *shadow, char *message);
int passthru_shadow_disconnect(passthru_shadow *shadow);

#endif