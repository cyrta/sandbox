import torch
import torchvision as tv


class ImagesDataset(torch.utils.data.Dataset):
    def __init__(self, df, transform=None,
                 loader=tv.datasets.folder.default_loader):
        self.df = df
        self.transform = transform
        self.loader = loader

    def __getitem__(self, index):
        row = self.df.iloc[index]

        target = row['class_']
        path = row['path']
        img = self.loader(path)
        if self.transform is not None:
            img = self.transform(img)

        return img, target

    def __len__(self):
        n, _ = self.df.shape
        return n

# what transformations should be done with our images
data_transforms = tv.transforms.Compose([
    tv.transforms.RandomCrop((64, 64), padding=4),
    tv.transforms.RandomHorizontalFlip(),
    tv.transforms.ToTensor(),
])

train_df = pd.read_csv('path/to/some.csv')
# initialize our dataset at first
train_dataset = ImagesDataset(
    df=train_df,
    transform=data_transforms
)

# initialize data loader with required number of workers and other params
train_loader = torch.utils.data.DataLoader(train_dataset,
                                           batch_size=10,
                                           shuffle=True,
                                           num_workers=16)

# fetch the batch(call to `__getitem__` method)
for img, target in train_loader:
    pass

