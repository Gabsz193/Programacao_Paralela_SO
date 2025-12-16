import subprocess
import os
import csv
import matplotlib.pyplot as plt

# Configuration
SIZES = [100000, 1000000, 10000000, 50000000]
THREADS = [1, 2, 4, 8, 16]
ITERATIONS = 3
OUTPUT_FILE = "out/sp-benchmark.csv"
EXECUTABLE = "./build/scalar_product"

def run_benchmark():
    results = []

    print(f"Running benchmarks with sizes: {SIZES} and threads: {THREADS}")
    
    # Sequential
    for size in SIZES:
        print(f"Running Sequential Size={size}")
        times = []
        for _ in range(ITERATIONS):
            cmd = [EXECUTABLE, "--size", str(size), "--mode", "seq"]
            result = subprocess.run(cmd, capture_output=True, text=True)
            if result.returncode != 0:
                print(f"Error: {result.stderr}")
                continue
            # Output format: mode,size,threads,time,result
            parts = result.stdout.strip().split(',')
            times.append(float(parts[3]))
        
        avg_time = sum(times) / len(times)
        results.append({"mode": "seq", "size": size, "threads": 1, "time": avg_time})

    # Parallel
    for size in SIZES:
        for t in THREADS:
            print(f"Running Parallel Size={size} Threads={t}")
            times = []
            for _ in range(ITERATIONS):
                cmd = [EXECUTABLE, "--size", str(size), "--mode", "par", "--threads", str(t)]
                result = subprocess.run(cmd, capture_output=True, text=True)
                if result.returncode != 0:
                    print(f"Error: {result.stderr}")
                    continue
                parts = result.stdout.strip().split(',')
                times.append(float(parts[3]))
            
            avg_time = sum(times) / len(times)
            results.append({"mode": "par", "size": size, "threads": t, "time": avg_time})
            
    return results

def save_results(results):
    with open(OUTPUT_FILE, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=["mode", "size", "threads", "time"])
        writer.writeheader()
        writer.writerows(results)

def generate_graphs(results):
    # Organize data
    seq_times = {r['size']: r['time'] for r in results if r['mode'] == 'seq'}
    
    # Speedup Graph
    plt.figure(figsize=(10, 6))
    for t in THREADS:
        speedups = []
        sizes = []
        for size in SIZES:
            par_time = next((r['time'] for r in results if r['mode'] == 'par' and r['size'] == size and r['threads'] == t), None)
            if par_time:
                speedup = seq_times[size] / par_time
                speedups.append(speedup)
                sizes.append(size)
        plt.plot(sizes, speedups, marker='o', label=f'{t} Threads')
    
    ax = plt.gca()
    ax.set_xticks(SIZES)

    plt.xscale('log')
    plt.xlabel('Tamanho')
    plt.ylabel('Speedup ($T_{seq} / T_{par}$)')
    plt.title('Speedup vs Tamanho')
    plt.legend()
    plt.grid(True)
    plt.savefig('out/sp-speedup.png')
    print("Generated sp-speedup.png")

    # Execution Time Graph
    plt.figure(figsize=(10, 6))
    
    # Plot Sequential
    sizes = sorted(list(seq_times.keys()))
    times = [seq_times[s] for s in sizes]
    plt.plot(sizes, times, marker='x', linestyle='--', label='Sequencial', color='black')

    for t in THREADS:
        times = []
        current_sizes = []
        for size in SIZES:
            par_time = next((r['time'] for r in results if r['mode'] == 'par' and r['size'] == size and r['threads'] == t), None)
            if par_time:
                times.append(par_time)
                current_sizes.append(size)
        plt.plot(current_sizes, times, marker='o', label=f'{t} Threads')

    ax = plt.gca()
    ax.set_xticks(SIZES)
    
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Tamanho')
    plt.ylabel('Tempo de Execução (s)')
    plt.title('Tempo de Execução vs Tamanho')
    plt.legend()
    plt.grid(True)
    plt.savefig('out/sp-execution_time.png')
    print("Generated sp-execution_time.png")

def print_table(results):
    print("\nBenchmark Results:")
    print(f"{'Mode':<10} {'Size':<15} {'Threads':<10} {'Time (s)':<15} {'Speedup':<10}")
    print("-" * 60)
    
    seq_times = {r['size']: r['time'] for r in results if r['mode'] == 'seq'}
    
    for r in results:
        speedup = 1.0
        if r['mode'] == 'par':
            speedup = seq_times[r['size']] / r['time']
        
        print(f"{r['mode']:<10} {r['size']:<15} {r['threads']:<10} {r['time']:<15.6f} {speedup:<10.2f}")

if __name__ == "__main__":
    print("Running benchmarks...")
    try:
        results = run_benchmark()
        save_results(results)
        print_table(results)
        generate_graphs(results)
    except Exception as e:
        print(f"An error occurred: {e}")
