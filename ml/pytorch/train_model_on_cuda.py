import torch

### tensor example
x_cpu = torch.randn(10, 20)
w_cpu = torch.randn(20, 10)
# direct transfer to the GPU
x_gpu = x_cpu.cuda()
w_gpu = w_cpu.cuda()
result_gpu = x_gpu @ w_gpu
# get back from GPU to CPU
result_cpu = result_gpu.cpu()

### model example
model = model.cuda()
# train step
inputs = Variable(inputs.cuda())
outputs = model(inputs)
# get back from GPU to CPU
outputs = outputs.cpu()
