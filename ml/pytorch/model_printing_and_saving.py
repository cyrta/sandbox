from collections import OrderedDict

import torch.nn as nn

model = nn.Sequential(OrderedDict([
    ('conv1', nn.Conv2d(1, 20, 5)),
    ('relu1', nn.ReLU()),
    ('conv2', nn.Conv2d(20, 64, 5)),
    ('relu2', nn.ReLU())
]))

print(model)

# Sequential (
#   (conv1): Conv2d(1, 20, kernel_size=(5, 5), stride=(1, 1))
#   (relu1): ReLU ()
#   (conv2): Conv2d(20, 64, kernel_size=(5, 5), stride=(1, 1))
#   (relu2): ReLU ()
# )

# save/load only the model parameters(prefered solution)
torch.save(model.state_dict(), save_path)
model.load_state_dict(torch.load(save_path))

# save whole model
torch.save(model, save_path)
model = torch.load(save_path)
