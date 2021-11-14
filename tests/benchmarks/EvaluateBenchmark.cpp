#include <benchmark/benchmark.h>
#include <Expression/Expression.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Evaluate.hpp>
#include <Expression/Ordering.hpp>
#include <Comutators/BosonComutators.hpp>
#include <Substitutions/Substitutions.hpp>
#include <transforms/BogoliubovTransform.hpp>






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
  Expression<Fock1DInfo> expression;
  const auto E = named_number<Fock1DInfo>('E');
  const auto Y = named_number<Fock1DInfo>('Y');
  for (int group = -2; group < state.range(0); ++group) {
    int index_0 = group;
    int index_1 = group - 1000;
    Expression<Fock1DInfo> grouped_expression = 
        (E *
            ((creation_op(index_0) *
              anihilation_op(index_0))
            + (creation_op(index_1) *
               anihilation_op(index_1)))

         + Y *
            ((creation_op(index_0) *
                creation_op(index_1))
            + anihilation_op(index_1) *
              anihilation_op(index_0)));
    expression = expression + grouped_expression;
  }
  for (auto _ : state) {
    auto result =
      bogoliubov_transform(expression, OperatorType::BOSON);
  }
}

BENCHMARK(bogoliubov_benchmark)->DenseRange(1, 5, 1);

BENCHMARK_MAIN();