// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cnn_network_impl.hpp"
#include "ie_memcpy.h"
#include "parsers.h"
#include "ie_util_internal.hpp"

namespace pugi {
class xml_node;

class xml_document;
}  // namespace pugi

namespace InferenceEngine {
namespace details {

struct FormatParserCreator {
    using Ptr = std::shared_ptr<FormatParserCreator>;
    virtual std::shared_ptr<IFormatParser> create(size_t version) = 0;
    virtual ~FormatParserCreator() = default;
};

struct INFERENCE_ENGINE_API_CLASS(V2FormatParserCreator) : public FormatParserCreator {
    std::shared_ptr<IFormatParser> create(size_t version) override;
};

class INFERENCE_ENGINE_API_CLASS(CNNNetReaderImpl) {
public:
    explicit CNNNetReaderImpl(const FormatParserCreator::Ptr& _creator);

    StatusCode ReadNetwork(const char* filepath, ResponseDesc* resp) noexcept;

    StatusCode ReadNetwork(const void* model, size_t size, ResponseDesc* resp) noexcept;

    StatusCode ReadNetwork(const pugi::xml_node& root, ResponseDesc* resp);

    StatusCode SetWeights(const TBlob<uint8_t>::Ptr& weights, ResponseDesc* resp) noexcept;

    StatusCode ReadWeights(const char* filepath, ResponseDesc* resp) noexcept;

    ICNNNetwork* getNetwork(ResponseDesc* resp) noexcept;

    std::shared_ptr<ICNNNetwork> getNetwork() {
        return network;
    }

    StatusCode getName(char* name, size_t len, ResponseDesc* resp) noexcept {
        if (len > 0) {
            size_t length = std::min(this->name.size(), len - 1);  // cut the name if buffer is too small
            ie_memcpy(name, len, this->name.c_str(), length);
            name[length] = '\0';  // null terminate
        }
        return OK;
    }

private:
    std::shared_ptr<InferenceEngine::details::IFormatParser> _parser;
    StatusCode ReadNetwork();

    std::string description;
    std::string name;
    std::shared_ptr<ICNNNetwork> network;
    bool parseSuccess;
    size_t _version;
    FormatParserCreator::Ptr parserCreator;

    // Stashed xmlDoc that is needed to delayed loading of V10 IR version
    std::shared_ptr<pugi::xml_document> xmlDoc;
};

}  // namespace details
}  // namespace InferenceEngine
