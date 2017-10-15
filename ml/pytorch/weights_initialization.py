import torch
from torch.autograd import Variable

# new way with `init` module
w = torch.Tensor(3, 5)
torch.nn.init.normal(w)
# work for Variables also
w2 = Variable(w)
torch.nn.init.normal(w2)
# old styled direct access to tensors data attribute
w2.data.normal_()

# example for some module
def weights_init(m):
    classname = m.__class__.__name__
    if classname.find('Conv') != -1:
        m.weight.data.normal_(0.0, 0.02)
    elif classname.find('BatchNorm') != -1:
        m.weight.data.normal_(1.0, 0.02)
        m.bias.data.fill_(0)

# for loop approach with direct access
class MyModel(nn.Module):
    def __init__(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                n = m.kernel_size[0] * m.kernel_size[1] * m.out_channels
                m.weight.data.normal_(0, math.sqrt(2. / n))
            elif isinstance(m, nn.BatchNorm2d):
                m.weight.data.fill_(1)
                m.bias.data.zero_()
            elif isinstance(m, nn.Linear):
                m.bias.data.zero_()
