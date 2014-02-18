/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: device.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "device.pb-c.h"
void   device__init
                     (Device         *message)
{
  static Device init_value = DEVICE__INIT;
  *message = init_value;
}
size_t device__get_packed_size
                     (const Device *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &device__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t device__pack
                     (const Device *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &device__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t device__pack_to_buffer
                     (const Device *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &device__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Device *
       device__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Device *)
     protobuf_c_message_unpack (&device__descriptor,
                                allocator, len, data);
}
void   device__free_unpacked
                     (Device *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &device__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor device__field_descriptors[3] =
{
  {
    "cid",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Device, cid),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "devid",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Device, devid),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "msg",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Device, msg),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned device__field_indices_by_name[] = {
  0,   /* field[0] = cid */
  1,   /* field[1] = devid */
  2,   /* field[2] = msg */
};
static const ProtobufCIntRange device__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor device__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "Device",
  "Device",
  "Device",
  "",
  sizeof(Device),
  3,
  device__field_descriptors,
  device__field_indices_by_name,
  1,  device__number_ranges,
  (ProtobufCMessageInit) device__init,
  NULL,NULL,NULL    /* reserved[123] */
};
