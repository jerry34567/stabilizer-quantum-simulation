import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib.font_manager as fm

# Read CSV file
df = pd.read_csv('benchmark-utils/results/edit_no-measure.csv')

# Convert '---', 'TO', 'Error' to NaN
df = df.replace(['---', 'TO', 'Error'], float('nan'))

# Convert strings to float
for col in df.columns[1:]:  # Skip 'Circuit' column
    df[col] = pd.to_numeric(df[col], errors='coerce')

# Create figure
plt.figure(figsize=(15, 8))

# Plot time comparison
plt.subplot(1, 2, 1)
plt.scatter(df['STABILIIZER t'], df['MEDUSA t'], alpha=0.5)
plt.xlabel('STABILIIZER Time (s)')
plt.ylabel('MEDUSA Time (s)')
plt.title('Execution Time Comparison')
plt.grid(True)
plt.xscale('log')
plt.yscale('log')

# Get valid pairs for time comparison
valid_time_df = df[df['STABILIIZER t'].notna() & df['MEDUSA t'].notna()]
max_time = max(valid_time_df['STABILIIZER t'].max(), valid_time_df['MEDUSA t'].max())
min_time = min(valid_time_df['STABILIIZER t'].min(), valid_time_df['MEDUSA t'].min())

plt.plot([min_time, max_time], [min_time, max_time], 'r--', alpha=0.5)
plt.xlim(min_time/1.1, max_time * 1.1)
plt.ylim(min_time/1.1, max_time * 1.1)

# Plot memory comparison
plt.subplot(1, 2, 2)
plt.scatter(df['STABILIIZER mem'], df['MEDUSA mem'], alpha=0.5)
plt.xlabel('STABILIIZER Memory (MB)')
plt.ylabel('MEDUSA Memory (MB)')
plt.title('Memory Usage Comparison')
plt.grid(True)
plt.xscale('log')
plt.yscale('log')

# Get valid pairs for memory comparison
valid_mem_df = df[df['STABILIIZER mem'].notna() & df['MEDUSA mem'].notna()]
max_mem = max(valid_mem_df['STABILIIZER mem'].max(), valid_mem_df['MEDUSA mem'].max())
min_mem = min(valid_mem_df['STABILIIZER mem'].min(), valid_mem_df['MEDUSA mem'].min())

plt.plot([min_mem, max_mem], [min_mem, max_mem], 'r--', alpha=0.5)
plt.xlim(min_mem/1.1, max_mem * 1.1)
plt.ylim(min_mem/1.1, max_mem * 1.1)

plt.tight_layout()
plt.savefig('comparison_plot.png')
plt.show()

# Output statistics summary
print("\nBasic Statistics:")
print(df.describe())

# Calculate average improvement ratios
time_speedup = df['STABILIIZER t'] / df['MEDUSA t']
mem_reduction = df['STABILIIZER mem'] / df['MEDUSA mem']

print("\nAverage Time Speedup:", time_speedup.mean())
print("Average Memory Reduction:", mem_reduction.mean())