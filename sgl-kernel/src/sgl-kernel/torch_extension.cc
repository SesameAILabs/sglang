#include <ATen/core/dispatch/Dispatcher.h>
#include <torch/library.h>

#include "sgl_kernels_ops.h"

TORCH_LIBRARY_EXPAND(sgl_kernels, m) {
  // trt_reduce
  m.def(
      "init_custom_ar(int rank_id, int world_size, Tensor rank_data, int[] buffers, int[] tmp_result_buffers, int[] "
      "barrier_in, int[] barrier_out) -> int");
  m.impl("init_custom_ar", torch::kCUDA, &init_custom_ar);

  m.def("dispose", &dispose);

  m.def("all_reduce(int fa, Tensor inp, Tensor! out) -> ()");
  m.impl("all_reduce", torch::kCUDA, &all_reduce);

  m.def("get_graph_buffer_ipc_meta(int fa) -> (int[], int[])");
  m.impl("get_graph_buffer_ipc_meta", torch::kCUDA, &get_graph_buffer_ipc_meta);

  m.def("register_graph_buffers(int fa, int[][] handles, int[][] offsets) -> ()");
  m.impl("register_graph_buffers", torch::kCUDA, &register_graph_buffers);

  // moe_align_block_size
  m.def(
      "moe_align_block_size(Tensor topk_ids, int num_experts, int block_size, Tensor! sorted_token_ids, Tensor! "
      "experts_ids, Tensor! num_tokens_post_pad, Tensor! token_cnts_buffer, Tensor! cumsum_buffer) -> ()");
  m.impl("moe_align_block_size", torch::kCUDA, &moe_align_block_size);

  // int8_scaled_mm
  m.def(
      "int8_scaled_mm(Tensor mat_a, Tensor mat_b, Tensor scales_a, Tensor scales_b, ScalarType out_dtype, Tensor? "
      "bias) -> Tensor");
  m.impl("int8_scaled_mm", torch::kCUDA, &int8_scaled_mm);

  // fp8_scaled_mm
  m.def(
      "fp8_scaled_mm(Tensor mat_a, Tensor mat_b, Tensor scales_a, Tensor scales_b, ScalarType out_dtype, Tensor? "
      "bias) -> Tensor");
  m.impl("fp8_scaled_mm", torch::kCUDA, &fp8_scaled_mm);

  // lightning_attention_decode
  m.def(
      "lightning_attention_decode(Tensor q, Tensor k, Tensor v, Tensor past_kv, Tensor slope, Tensor! output, Tensor! "
      "new_kv) -> ()");
  m.impl("lightning_attention_decode", torch::kCUDA, &lightning_attention_decode);

  // rms norm
  m.def("rmsnorm(Tensor! output, Tensor input, Tensor weight, float eps, int cuda_stream) -> ()");
  m.impl("rmsnorm", torch::kCUDA, &rmsnorm);

  // fused rms norm
  m.def("fused_add_rmsnorm(Tensor! input, Tensor! residual, Tensor weight, float eps) -> ()");
  m.impl("fused_add_rmsnorm", torch::kCUDA, &sgl_fused_add_rmsnorm);

  // gemma rms norm
  m.def("gemma_rmsnorm(Tensor! output, Tensor input, Tensor weight, float eps, int cuda_stream) -> ()");
  m.impl("gemma_rmsnorm", torch::kCUDA, &gemma_rmsnorm);

  // fused gemma rms norm
  m.def("gemma_fused_add_rmsnorm(Tensor! input, Tensor! residual, Tensor weight, float eps, int cuda_stream) -> ()");
  m.impl("gemma_fused_add_rmsnorm", torch::kCUDA, &gemma_fused_add_rmsnorm);

  // silu and mul
  m.def("silu_and_mul(Tensor! out, Tensor input, int cuda_stream) -> ()");
  m.impl("silu_and_mul", torch::kCUDA, &silu_and_mul);

  // gelu tanh and mul
  m.def("gelu_tanh_and_mul(Tensor! out, Tensor input, int cuda_stream) -> ()");
  m.impl("gelu_tanh_and_mul", torch::kCUDA, &gelu_tanh_and_mul);

  // gelu and mul
  m.def("gelu_and_mul(Tensor! out, Tensor input, int cuda_stream) -> ()");
  m.impl("gelu_and_mul", torch::kCUDA, &gelu_and_mul);

  // bmm fp8
  m.def(
      "bmm_fp8(Tensor A, Tensor B, Tensor! D, Tensor A_scale, Tensor B_scale, Tensor workspace_buffer, int "
      "cublas_handle, int cuda_stream) -> ()");
  m.impl("bmm_fp8", torch::kCUDA, &bmm_fp8);

  // min p sampling from probs
  m.def(
      "min_p_sampling_from_probs(Tensor probs, Tensor uniform_samples, Tensor! samples, Tensor? maybe_min_p_arr, float "
      "min_p_val, bool deterministic, int cuda_stream) -> ()");
  m.impl("min_p_sampling_from_probs", torch::kCUDA, &min_p_sampling_from_probs);

  // top k renorm probs
  m.def(
      "top_k_renorm_probs_wrapper(Tensor probs, Tensor! renorm_probs, Tensor? maybe_top_k_arr, int top_k_val, int "
      "cuda_stream) -> ()");
  m.impl("top_k_renorm_probs_wrapper", torch::kCUDA, &top_k_renorm_probs_wrapper);

  // top p renorm probs
  m.def(
      "top_p_renorm_probs(Tensor probs, Tensor! renorm_probs, Tensor? maybe_top_p_arr, float top_p_val, int "
      "cuda_stream) -> ()");
  m.impl("top_p_renorm_probs", torch::kCUDA, &top_p_renorm_probs);

  // top k top p sampling from probs
  m.def(
      "top_k_top_p_sampling_from_probs(Tensor probs, Tensor uniform_samples, Tensor! samples, Tensor! success, Tensor? "
      "maybe_top_k_arr, float top_k_val, Tensor? maybe_top_p_arr, float top_p_val, bool deterministic, int "
      "cuda_stream) -> ()");
  m.impl("top_k_top_p_sampling_from_probs", torch::kCUDA, &top_k_top_p_sampling_from_probs);

  // top p sampling from probs
  m.def(
      "top_p_sampling_from_probs(Tensor probs, Tensor uniform_samples, Tensor! samples, Tensor! success, Tensor? "
      "maybe_top_p_arr, float top_p_val, bool deterministic, int cuda_stream) -> ()");
  m.impl("top_p_sampling_from_probs", torch::kCUDA, &top_p_sampling_from_probs);

  // apply rope with cos sin cache
  m.def(
      "apply_rope_pos_ids_cos_sin_cache(Tensor q, Tensor k, Tensor! q_rope, Tensor! k_rope, Tensor cos_sin_cache, "
      "Tensor pos_ids, bool interleave, int cuda_stream) -> ()");
  m.impl("apply_rope_pos_ids_cos_sin_cache", torch::kCUDA, &apply_rope_pos_ids_cos_sin_cache);
}

REGISTER_EXTENSION(_kernels)
