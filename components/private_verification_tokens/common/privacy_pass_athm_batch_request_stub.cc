// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// No Crubit: ATHM is unavailable, every request fails.

#include "components/private_verification_tokens/common/privacy_pass_athm_batch_request.h"

#include <vector>

namespace private_verification_tokens {

// static
base::expected<PrivacyPassAthmBatchRequest, PrivacyPassAthmBatchRequestError>
PrivacyPassAthmBatchRequest::Create(const IssuerConfig& issuer_config,
                                    uint8_t num_buckets) {
  if (issuer_config.batch_size <= 0) {
    return base::unexpected(
        PrivacyPassAthmBatchRequestError::kInvalidBatchSize);
  }
  if (num_buckets == 0) {
    return base::unexpected(
        PrivacyPassAthmBatchRequestError::kInvalidBucketCount);
  }
  return base::unexpected(
      PrivacyPassAthmBatchRequestError::kClientRequestGenerationFailed);
}

PrivacyPassAthmBatchRequest::PrivacyPassAthmBatchRequest(
    PrivacyPassAthmBatchRequest&&) = default;
PrivacyPassAthmBatchRequest& PrivacyPassAthmBatchRequest::operator=(
    PrivacyPassAthmBatchRequest&&) = default;
PrivacyPassAthmBatchRequest::~PrivacyPassAthmBatchRequest() = default;

base::expected<std::vector<std::vector<uint8_t>>,
               PrivacyPassAthmBatchRequestError>
PrivacyPassAthmBatchRequest::Finalize(base::span<const uint8_t> response_body) {
  return base::unexpected(
      PrivacyPassAthmBatchRequestError::kClientFinalizeFailed);
}

}  // namespace private_verification_tokens
