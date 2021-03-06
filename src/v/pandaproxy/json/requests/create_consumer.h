/*
 * Copyright 2020 Vectorized, Inc.
 *
 * Use of this software is governed by the Business Source License
 * included in the file licenses/BSL.md
 *
 * As of the Change Date specified in that file, in accordance with
 * the Business Source License, use of this software will be governed
 * by the Apache License, Version 2.0
 */

#pragma once

#include "bytes/iobuf.h"
#include "json/json.h"
#include "kafka/protocol/errors.h"
#include "kafka/protocol/produce.h"
#include "kafka/types.h"
#include "pandaproxy/json/iobuf.h"
#include "seastarx.h"
#include "utils/string_switch.h"

#include <seastar/core/sstring.hh>

#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string_view>

namespace pandaproxy::json {

struct create_consumer_request {
    std::optional<ss::sstring> name;
    ss::sstring format{"binary"};
    ss::sstring auto_offset_reset;
    ss::sstring auto_commit_enable;
    ss::sstring fetch_min_bytes;
    ss::sstring consumer_request_timeout_ms;
};

template<typename Encoding = rapidjson::UTF8<>>
class create_consumer_request_handler {
private:
    enum class state {
        empty = 0,
        name,
        format,
        auto_offset_reset,
        auto_commit_enable,
        fetch_min_bytes,
        timeout_ms
    };

    state _state = state::empty;

public:
    using Ch = typename Encoding::Ch;
    using rjson_parse_result = create_consumer_request;
    rjson_parse_result result;

    bool Null() { return false; }
    bool Bool(bool) { return false; }
    bool Int64(int64_t) { return false; }
    bool Uint64(uint64_t) { return false; }
    bool Double(double) { return false; }
    bool RawNumber(const Ch*, rapidjson::SizeType, bool) { return false; }
    bool Int(int) { return false; }
    bool Uint(unsigned) { return false; }
    bool StartArray() { return false; }
    bool EndArray(rapidjson::SizeType) { return false; }

    bool String(const Ch* str, rapidjson::SizeType len, bool) {
        switch (_state) {
        case state::empty:
            return false;
        case state::name:
            result.name = {str, len};
            break;
        case state::format:
            result.format = {str, len};
            break;
        case state::auto_offset_reset:
            result.auto_offset_reset = {str, len};
            break;
        case state::auto_commit_enable:
            result.auto_commit_enable = {str, len};
            break;
        case state::fetch_min_bytes:
            result.fetch_min_bytes = {str, len};
            break;
        case state::timeout_ms:
            result.consumer_request_timeout_ms = {str, len};
            break;
        }
        return true;
    }

    bool Key(const char* str, rapidjson::SizeType len, bool) {
        _state = string_switch<state>({str, len})
                   .match("name", state::name)
                   .match("format", state::format)
                   .match("auto.offset.reset", state::auto_offset_reset)
                   .match("auto.commit.enable", state::auto_commit_enable)
                   .match("fetch.min.bytes", state::fetch_min_bytes)
                   .match("consumer.request.timeout.ms", state::timeout_ms)
                   .default_match(state::empty);

        return _state != state::empty;
    }

    bool StartObject() { return _state == state::empty; }

    bool EndObject(rapidjson::SizeType) { return _state != state::empty; }
};

struct create_consumer_response {
    kafka::member_id instance_id;
    ss::sstring base_uri;
};

inline void rjson_serialize(
  rapidjson::Writer<rapidjson::StringBuffer>& w,
  const create_consumer_response& res) {
    w.StartObject();
    w.Key("instance_id");
    w.String(res.instance_id());
    w.Key("base_uri");
    w.String(res.base_uri);
    w.EndObject();
}

template<typename Encoding = rapidjson::UTF8<>>
class create_consumer_response_handler {
private:
    enum class state { empty = 0, instance_id, base_uri };

    state _state = state::empty;

public:
    using Ch = typename Encoding::Ch;
    using rjson_parse_result = create_consumer_response;
    rjson_parse_result result;

    bool Null() { return false; }
    bool Bool(bool) { return false; }
    bool Int64(int64_t) { return false; }
    bool Uint64(uint64_t) { return false; }
    bool Double(double) { return false; }
    bool RawNumber(const Ch*, rapidjson::SizeType, bool) { return false; }
    bool Int(int) { return false; }
    bool Uint(unsigned) { return false; }
    bool StartArray() { return false; }
    bool EndArray(rapidjson::SizeType) { return false; }

    bool String(const Ch* str, rapidjson::SizeType len, bool) {
        auto str_view{std::string_view{str, len}};
        switch (_state) {
        case state::empty:
            return false;
        case state::instance_id:
            result.instance_id = kafka::member_id{ss::sstring{str_view}};
            break;
        case state::base_uri:
            result.base_uri = {str, len};
            break;
        }
        return true;
    }

    bool Key(const char* str, rapidjson::SizeType len, bool) {
        _state = string_switch<state>({str, len})
                   .match("instance_id", state::instance_id)
                   .match("base_uri", state::base_uri)
                   .default_match(state::empty);

        return _state != state::empty;
    }

    bool StartObject() { return _state == state::empty; }

    bool EndObject(rapidjson::SizeType) { return _state != state::empty; }
};

} // namespace pandaproxy::json
