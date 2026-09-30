#ifndef PINYIN_ENRICHER_H
#define PINYIN_ENRICHER_H

#include "cedict.hpp"
#include <variant>
#include <optional>
#include <deque>
#include <string_view>
#include <string>

struct ErrorCodeAndDetail;
struct VocItem;

std::variant<std::string_view, ErrorCodeAndDetail> formatPinyin(const VocItem& vocItem, std::deque<std::string>& pool);

// WARN: should be called only if vocItem is formatted
std::optional<ErrorCodeAndDetail> checkPinyinValidity(VocItem& formattedVocItem, const DictEntries& entries);


#endif