# scheduler example
from torch.optim import lr_scheduler

optimizer = torch.optim.SGD(model.parameters(), lr=0.01)
scheduler = lr_scheduler.StepLR(optimizer, step_size=30, gamma=0.1)

for epoch in range(100):
    scheduler.step()
    train()
    validate()

# Train flag can be updated with boolean
# to disable dropout and batch norm learning
model.train(True)
# execute train step
model.train(False)
# run inference step

# CPU seed
torch.manual_seed(42)
# GPU seed
torch.cuda.manual_seed_all(42)
