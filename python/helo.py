import pandas as pd
import matplotlib.pyplot as plt

file_path = r"D:\\UserData\\Parminder\Documents\\hi.csv"
df = pd.read_csv(file_path, skiprows = 1)
averages = df.mean()
print("Averages of each column:")
for i, avg in enumerate(averages):
    print( f"Column{1+i}: {avg}")
k = 0
for i in df.columns:
    print(df[i]-averages[i])
    plt.plot(df[i]-averages[i])
    k=k+1
print(k)
plt.xlabel("Index")
plt.ylabel("value")
plt.title("Acceleration in X,Y,Z")
plt.show()