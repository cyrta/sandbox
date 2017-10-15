import torch
import numpy as np

numpy_tensor = np.random.randn(10, 20)

# convert numpy array to pytorch array
pytorch_tensor = torch.Tensor(numpy_tensor)
# or another way
pytorch_tensor = torch.from_numpy(numpy_tensor)

# convert torch tensor to numpy representation
pytorch_tensor.numpy()

# if we want to use tensor on GPU provide another type
dtype = torch.cuda.FloatTensor
gpu_tensor = torch.randn(10, 20).type(dtype)
# or just call `cuda()` method
gpu_tensor = pytorch_tensor.cuda()
# call back to the CPU
cpu_tensor = gpu_tensor.cpu()

# define pytorch tensors
x = torch.randn(10, 20)
y = torch.ones(20, 5)
# `@` mean matrix multiplication from python3.5, PEP-0465
res = x @ y

# get the shape
res.shape  # torch.Size([10, 5])
