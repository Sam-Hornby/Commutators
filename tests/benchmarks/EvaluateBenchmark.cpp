#include <benchmark/benchmark.h>
#include <Expression/Expression.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Evaluate.hpp>
#include <Expression/Ordering.hpp>
#include <Comutators/BosonComutators.hpp>
#include <Substitutions/Substitutions.hpp>





using namespace operators;

static void evaluate_benchmark(benchmark::State& state) {
  Expression<Fock1DInfo> expression =
      normalised_n_occupied_ops<Fock1DInfo>(2 * state.range(0), 0) *
      normalised_n_occupied_ops<Fock1DInfo>(3 * state.range(1), 1) *
      vacuum_state<Fock1DInfo>();

  expression = hermition_conjugate<Fock1DInfo>(expression) *
               expression;

  for (auto _ : state) {
    expression = normal_order(expression);
    expression = evaluate<Fock1DInfo>(expression,
                          boson_commutator<Fock1DInfo>,
                          default_boson_subs<Fock1DInfo>);
  }
}

static void custom_arguments(benchmark::internal::Benchmark* b) {
  for (unsigned i = 1; i < 4; ++i) {
    for (unsigned j = 1; j < 3; ++j) {
      b->Args({i, j});
    }
  }
}
// Register the function as a benchmark
BENCHMARK(evaluate_benchmark)->Apply(custom_arguments);


static void bogoliubov_benchmark(benchmark::State& state) {
  
}

BENCHMARK(bogoliubov_benchmark);

BENCHMARK_MAIN();