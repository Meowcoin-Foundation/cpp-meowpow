// meowpow: C/C++ implementation of Meowpow, the Meowcoin Proof of Work algorithm.
// Copyright 2018-2019 Pawel Bylica.
// Licensed under the Apache License, Version 2.0.

#include "../unittests/helpers.hpp"

#include <meowpow/meowpow-internal.hpp>
#include <meowpow/keccak.hpp>
#include <meowpow/primes.h>

#include <benchmark/benchmark.h>


static void calculate_light_cache_num_items(benchmark::State& state)
{
    const auto epoch_number = static_cast<int>(state.range(0));

    for (auto _ : state)
    {
        auto answer = meowpow::calculate_light_cache_num_items(epoch_number);
        benchmark::DoNotOptimize(&answer);
    }
}
BENCHMARK(calculate_light_cache_num_items)->Arg(32638)->Arg(32639);

static void calculate_full_dataset_num_items(benchmark::State& state)
{
    const auto epoch_number = static_cast<int>(state.range(0));

    for (auto _ : state)
    {
        auto answer = meowpow::calculate_full_dataset_num_items(epoch_number);
        benchmark::DoNotOptimize(&answer);
    }
}
BENCHMARK(calculate_full_dataset_num_items)->Arg(32638)->Arg(32639);


static void seed(benchmark::State& state)
{
    const int epoch_number = static_cast<int>(state.range(0));

    for (auto _ : state)
    {
        auto seed = meowpow::calculate_epoch_seed(epoch_number);
        benchmark::DoNotOptimize(seed.bytes);
    }
}
BENCHMARK(seed)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);


static void light_cache(benchmark::State& state)
{
    const int epoch_number = static_cast<int>(state.range(0));
    const auto num_items = meowpow::calculate_light_cache_num_items(epoch_number);
    const auto seed = meowpow::calculate_epoch_seed(epoch_number);

    std::unique_ptr<meowpow::hash512[]> light_cache{new meowpow::hash512[num_items]};

    for (auto _ : state)
    {
        meowpow::build_light_cache(light_cache.get(), num_items, seed);
        benchmark::DoNotOptimize(light_cache.get());
    }
}
BENCHMARK(light_cache)->Arg(1)->Unit(benchmark::kMillisecond);


static void meowpow_calculate_dataset_item_512(benchmark::State& state)
{
    auto& ctx = get_meowpow_epoch_context_0();

    for (auto _ : state)
    {
        auto item = meowpow::calculate_dataset_item_512(ctx, 1234);
        benchmark::DoNotOptimize(item.bytes);
    }
}
BENCHMARK(meowpow_calculate_dataset_item_512);


static void meowpow_calculate_dataset_item_1024(benchmark::State& state)
{
    auto& ctx = get_meowpow_epoch_context_0();

    for (auto _ : state)
    {
        auto item = meowpow::calculate_dataset_item_1024(ctx, 1234);
        benchmark::DoNotOptimize(item.bytes);
    }
}
BENCHMARK(meowpow_calculate_dataset_item_1024);


static void meowpow_calculate_dataset_item_2048(benchmark::State& state)
{
    auto& ctx = get_meowpow_epoch_context_0();

    for (auto _ : state)
    {
        auto item = meowpow::calculate_dataset_item_2048(ctx, 1234);
        benchmark::DoNotOptimize(item.bytes);
    }
}
BENCHMARK(meowpow_calculate_dataset_item_2048);


static void meowpow_hash(benchmark::State& state)
{
    // Get block number in millions.
    const int block_number = static_cast<int>(state.range(0)) * 1000000;
    uint64_t nonce = 1;

    const auto& ctx = meowpow::get_global_epoch_context(meowpow::get_epoch_number(block_number));

    for (auto _ : state)
        meowpow::hash(ctx, {}, nonce++);
}
BENCHMARK(meowpow_hash)->Unit(benchmark::kMicrosecond)->Arg(0)->Arg(10);


static void verify(benchmark::State& state)
{
    const int block_number = 5000000;
    const meowpow::hash256 header_hash =
        to_hash256("bc544c2baba832600013bd5d1983f592e9557d04b0fb5ef7a100434a5fc8d52a");
    const meowpow::hash256 mix_hash =
        to_hash256("94cd4e844619ee20989578276a0a9046877d569d37ba076bf2e8e34f76189dea");
    const uint64_t nonce = 0x4617a20003ba3f25;
    const meowpow::hash256 boundry =
        to_hash256("0000000000001a5c000000000000000000000000000000000000000000000000");

    static const auto ctx = meowpow::create_epoch_context(meowpow::get_epoch_number(block_number));

    for (auto _ : state)
        meowpow::verify(*ctx, header_hash, mix_hash, nonce, boundry);
}
BENCHMARK(verify);


static void verify_mt(benchmark::State& state)
{
    const int block_number = 5000000;
    const meowpow::hash256 header_hash =
        to_hash256("bc544c2baba832600013bd5d1983f592e9557d04b0fb5ef7a100434a5fc8d52a");
    const meowpow::hash256 mix_hash =
        to_hash256("94cd4e844619ee20989578276a0a9046877d569d37ba076bf2e8e34f76189dea");
    const uint64_t nonce = 0x4617a20003ba3f25;
    const meowpow::hash256 boundry =
        to_hash256("0000000000001a5c000000000000000000000000000000000000000000000000");

    static const auto ctx = meowpow::create_epoch_context(meowpow::get_epoch_number(block_number));

    for (auto _ : state)
        meowpow::verify(*ctx, header_hash, mix_hash, nonce, boundry);
}
BENCHMARK(verify_mt)->Threads(1)->Threads(2)->Threads(4)->Threads(8);


static void verify_managed(benchmark::State& state)
{
    const int block_number = 5000000;
    const meowpow::hash256 header_hash =
        to_hash256("bc544c2baba832600013bd5d1983f592e9557d04b0fb5ef7a100434a5fc8d52a");
    const meowpow::hash256 mix_hash =
        to_hash256("94cd4e844619ee20989578276a0a9046877d569d37ba076bf2e8e34f76189dea");
    const uint64_t nonce = 0x4617a20003ba3f25;
    const meowpow::hash256 boundry =
        to_hash256("0000000000001a5c000000000000000000000000000000000000000000000000");

    const int epoch_number = meowpow::get_epoch_number(block_number);

    // This should create the light cache.
    meowpow::get_global_epoch_context(epoch_number);

    for (auto _ : state)
    {
        auto& context = meowpow::get_global_epoch_context(epoch_number);
        meowpow::verify(context, header_hash, mix_hash, nonce, boundry);
    }
}
BENCHMARK(verify_managed)->Threads(1)->Threads(2)->Threads(4)->Threads(8);


BENCHMARK_MAIN();
