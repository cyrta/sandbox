class Trainer:
    def __init__(self, model, use_cuda=False, gpu_idx=0):
        self.use_cuda = use_cuda
        self.gpu_idx = gpu_idx
        self.model = self.to_gpu(model)

    def to_gpu(self, tensor):
        if self.use_cuda:
            return tensor.cuda(self.gpu_idx)
        else:
            return tensor

    def from_gpu(self, tensor):
        if self.use_cuda:
            return tensor.cpu()
        else:
            return tensor

    def train(self, inputs):
        inputs = self.to_gpu(inputs)
        outputs = self.model(inputs)
        outputs = self.from_gpu(outputs)
