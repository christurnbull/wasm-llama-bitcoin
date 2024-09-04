#include "common.h"
#include "llama.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

class LlamaCpp
{
public:
    void init(int32_t n_threads)
    {
        _init(n_threads);
    }

    void load_model(const std::string &model_id)
    {
        model = llama_load_model_from_file(model_id.c_str(), model_params);
        if (model == NULL)
        {
            fprintf(stderr, "%s: error: unable to load model\n", __func__);
        }
    }

    std::string tokenize(const std::string prompt)
    {
        _initContext();
        return _tokenize(prompt);
    }

    std::string predictCurrentTokens()
    {
        _evaluatePrompt();
        return _inferenceLoop();
    }

    std::string predict(const std::string &prompt)
    {
        _initContext();
        _tokenize(prompt);
        _evaluatePrompt();
        return _inferenceLoop();
    }

    void destroy()
    {
        LOG_TEE("%s: llama_batch_free\n", __func__);
        if (batch.n_tokens > 0)
        {
            llama_batch_free(batch);
        }

        LOG_TEE("%s: llama_free\n", __func__);
        if (has_ctx)
        {
            llama_free(ctx);
            has_ctx = false;
        }

        LOG_TEE("%s: llama_free_model\n", __func__);
        if (model != nullptr)
        {
            llama_free_model(model);
        }

        LOG_TEE("%s: llama_backend_free\n", __func__);
        llama_backend_free();
    }

private:
    llama_model_params model_params;
    llama_model *model;
    llama_context_params ctx_params;
    llama_batch batch;
    llama_context *ctx;
    bool has_ctx;
    int32_t n_threads;
    std::vector<llama_token> tokens_list;
    int n_len;

    void _init(int32_t n_threads_in)
    {
        n_len = 44;
        n_threads = n_threads_in;
        // n_threads = std::max(cpu_get_num_math() - 1, 1);
        LOG_TEE("%s: n_threads = %d\n", __func__, n_threads);

        llama_backend_init();

        model_params = llama_model_default_params();
        model_params.use_mmap = false;

        batch.n_tokens = 0;
        has_ctx = false;
    }

    void _initContext()
    {
        if (has_ctx)
        {
            llama_free(ctx);
            has_ctx = false;
        }
        ctx_params = llama_context_default_params();
        ctx_params.seed = 42;
        ctx_params.n_ctx = llama_n_ctx_train(model);
        // ctx_params.n_ctx = 300;
        ctx_params.n_threads = n_threads;
        ctx_params.n_threads_batch = n_threads;
        ctx_params.flash_attn = true;

        ctx = llama_new_context_with_model(model, ctx_params);
        has_ctx = true;
    }

    std::string _tokenize(const std::string prompt)
    {
#ifdef EMSCRIPTEN
        EM_ASM(
            // clang-format off
            self.postMessage({em_cmd: 'msg', data: {
                text: 'Tokenizing... ', 
                type: 'INFO',
                pending: true
            }});
            // clang-format on
        );
#endif
        tokens_list = llama_tokenize(ctx, prompt, true);

        const int n_ctx = llama_n_ctx(ctx);

        if (tokens_list.size() > n_ctx)
        {
            tokens_list.resize(n_ctx);
        }
        LOG_TEE("%s: tokens_list = %d\n", __func__, (int)tokens_list.size());

        std::string tokenizedPrompt = "";
        for (int i = 0; i < tokens_list.size(); i++)
        {
            int token_id = tokens_list[i];
            tokenizedPrompt += llama_token_to_piece(ctx, token_id);
        }

#ifdef EMSCRIPTEN
        EM_ASM(
            // clang-format off
            self.postMessage({em_cmd: 'msgAppend', data: {
                text: 'Done.', pending: false
            }});
            // clang-format on
        );
#endif

        return tokenizedPrompt;

        // const int n_kv_req = tokens_list.size() + (n_len - tokens_list.size());

        // LOG_TEE("\n%s: n_len = %d, n_ctx = %d, n_kv_req = %d\n", __func__, n_len, n_ctx, n_kv_req);

        // // make sure the KV cache is big enough to hold all the prompt and generated tokens
        // if (n_kv_req > n_ctx)
        // {
        //     LOG_TEE("%s: error: n_kv_req > n_ctx, the required KV cache size is not big enough\n", __func__);
        //     LOG_TEE("%s:        either reduce n_len or increase n_ctx\n", __func__);
        //     return;
        // }
    }

    void _evaluatePrompt()
    {
#ifdef EMSCRIPTEN
        EM_ASM(
            // clang-format off
            self.postMessage({em_cmd: 'msg', data: {
                text: 'Decoding prompt... ', 
                type: 'INFO',
                pending: true
            }});
            // clang-format on
        );
#endif

        LOG_TEE("%s\n", __func__);
        if (batch.n_tokens > 0)
        {
            llama_batch_free(batch);
        }

        // create a llama_batch with size 512
        // we use this object to submit token data for decoding
        batch = llama_batch_init(512, 0, 1);

        // evaluate the initial prompt
        for (size_t i = 0; i < tokens_list.size(); i++)
        {
            llama_batch_add(batch, tokens_list[i], i, {0}, false);
        }

        // llama_decode will output logits only for the last token of the prompt
        batch.logits[batch.n_tokens - 1] = true;

        int32_t decoded = llama_decode(ctx, batch);
        if (decoded != 0)
        {
            LOG_TEE("%s: llama_decode() failed\n", __func__);
        }

#ifdef EMSCRIPTEN
        EM_ASM(
            // clang-format off
            self.postMessage({em_cmd: 'msgAppend', data: {
                text: 'Done.', pending: false
            }});
            // clang-format on
        );
#endif
    }

    std::string _inferenceLoop()
    {
#ifdef EMSCRIPTEN
        EM_ASM(
            // clang-format off
            self.postMessage({em_cmd: 'msg', data: {
                text: 'Predicting tokens: ', 
                type: 'INFO',
                pending: true
            }});
            // clang-format on
        );
#endif

        LOG_TEE("%s\n", __func__);
        std::string predicted = "";
        int n_cur = batch.n_tokens;
        int n_decode = 0;

        // LOG_TEE("%d\n", n_cur);
        while (predicted.length() < n_len)
        {
            // LOG_TEE("%s\n", predicted.c_str());

            // sample the next token
            {
                auto n_vocab = llama_n_vocab(model);
                auto *logits = llama_get_logits_ith(ctx, batch.n_tokens - 1);

                std::vector<llama_token_data> candidates;
                candidates.reserve(n_vocab);

                for (llama_token token_id = 0; token_id < n_vocab; token_id++)
                {
                    candidates.emplace_back(llama_token_data{token_id, logits[token_id], 0.0f});
                }

                llama_token_data_array candidates_p = {candidates.data(), candidates.size(), false};

                // sample the most likely token
                const llama_token new_token_id = llama_sample_token_greedy(ctx, &candidates_p);

                // is it an end of stream?
                // if (new_token_id == llama_token_eos(model) || n_cur == n_len)
                if (new_token_id == llama_token_eos(model) || n_cur == n_len || predicted.length() == n_len)
                {
                    LOG_TEE("\n");
                    break;
                }

                std::string new_token = llama_token_to_piece(ctx, new_token_id);

                predicted += new_token;

                // LOG_TEE("%s", llama_token_to_piece(ctx, new_token_id).c_str());
                // fflush(stdout);

#ifdef EMSCRIPTEN
                // clang-format off
                EM_ASM({
                    self.postMessage({em_cmd: 'msgAppend', data: {
                        text: UTF8ToString($0)
                    }});
                }, new_token.c_str());
                // clang-format on
#endif

                // prepare the next batch
                llama_batch_clear(batch);

                // push this new token for next evaluation
                llama_batch_add(batch, new_token_id, n_cur, {0}, true);

                n_decode += 1;
            }

            n_cur += 1;

            // evaluate the current batch with the transformer model
            if (llama_decode(ctx, batch))
            {
                fprintf(stderr, "%s : failed to eval, return code %d\n", __func__, 1);
                return predicted;
            }
        }
        // LOG_TEE("\n%s\n\n", predicted.c_str());

#ifdef EMSCRIPTEN
        EM_ASM(
            // clang-format off
            self.postMessage({em_cmd: 'msgAppend', data: {
                pending: false
            }});
            // clang-format on
        );
#endif
        return predicted;
    }
};