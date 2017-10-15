
#https://cdn-images-1.medium.com/max/1600/1*A-cWYNur2lqDEhUF1_gdCw.png

class ImagesDataset(torch.utils.data.Dataset)
    pass

class Net(nn.Module):
    pass

model = Net()
optimizer = torch.optim.SGD(model.parameters(), lr=0.01)
scheduler = lr_scheduler.StepLR(optimizer, step_size=30, gamma=0.1)
criterion = torch.nn.MSELoss()

dataset = ImagesDataset(path_to_images)
data_loader = torch.utils.data.DataLoader(dataset, batch_size=10)

train = True
for epoch in range(epochs):
    if train:
        lr_scheduler.step()

    for inputs, labels in data_loader:
        inputs = Variable(to_gpu(inputs))
        labels = Variable(to_gpu(labels))

        outputs = model(inputs)
        loss = criterion(outputs, labels)
        if train:
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

    if not train:
        save_best_model(epoch_validation_accuracy)
