set(ml_executables_list
  MNIST_AAE_example
  MNIST_AAE_LabelStyle_example
  MNIST_CovNet_example
  MNIST_EvoNet_example
  MNIST_GAN_example
  MNIST_LSTM_example
  MNIST_VAE_example
  AdditionProblem_example
  MemoryCell_example
  Metabolomics_example
)

set(cuda_executables_list
  CUDA_example
)

### collect example executables
set(EXAMPLE_executables
  ${ml_executables_list}
  ${cuda_executables_list}
)
