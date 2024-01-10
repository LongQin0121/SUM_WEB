
# High-Performance Parallel Optimization of the Fish School Behaviour on the Setonix Platform Using OpenMP

### Abstract

This paper presents an in-depth investigation into the high-performance parallel optimization of the Fish School Behaviour (FSB) algorithm on the Setonix supercomputing platform using the OpenMP framework. The FSB algorithm, inspired by nature's social behavior patterns, provides an ideal platform for parallelization due to its iterative and computationally intensive nature. The study leverages the capabilities of the Setonix platform and the OpenMP framework to analyze various aspects of multi-threading, aiming to discern patterns and strategies that can elevate program performance.

### How to run  the code?
In Linux Environment:

###gcc -fopenmp -o executable_name source_code.c

###./executable_name

### 1. Introduction

#### 1.1 Background

With the rapid advancements in scientific research and technology, there has been an escalating demand for computational power, especially for large-scale and intricate calculations. High-performance computing (HPC) plays an instrumental role by enhancing computational speeds through optimized parallel algorithms and frameworks. The Fish School Behaviour (FSB) algorithm, a variant of Particle Swarm Optimization (PSO), stands out as an ideal candidate for parallelization, offering a robust experimental platform for HPC research.

#### 1.2 Objectives

The central objective is to harness the computational prowess of the Setonix platform and the OpenMP framework to conduct high-performance parallel optimization of the FSB algorithm. The study delves deep into the impact of various multi-threading facets on program performance, examining thread counts, scheduling strategies, and OpenMP constructs.

### 2. Methodology

#### 2.1 Experimental System Configuration

Ensuring consistent starting conditions across experiments is pivotal to warranting the comparability of the results. The initialization and characteristics of the fish population, pond simulation environment configuration, and system parameter configuration are outlined.

#### 2.2 Parallelization Strategy

To enhance simulation efficiency, the OpenMP framework is employed. The introduction to the OpenMP framework and the definition and selection of independent variables, including thread count, scheduling strategy, block size, and parallel structure, are discussed.

#### 2.3 Experimental Design

Utilizing the OpenMP parallel computing paradigm, critical independent variables are scrutinized to discern their impact on holistic system performance. Benchmark settings and three experiments focusing on thread count, scheduling strategy, and parallel constructs are detailed.

### 3. Experimental Results and Data Analysis

#### 3.1 Experimental Configuration

The configuration for the experimental environment, including operating system information, kernel version, and compiler version, is provided.

#### 3.2 Experiment 1: Influence of Thread Count on Program Runtime

This experiment investigates the variation in program runtime under different thread counts, particularly with constant scheduling strategies and parallel structures. The impact of Reduction Construct vs. Critical Construct with Static Scheduling and Static vs. Dynamic vs. Guided with Reduction Structure is analyzed.

#### 3.3 Experiment 2: Influence of Scheduling Strategy on Program Runtime

This section examines the impact of varying scheduling strategies and chunk values on program runtime, keeping thread count and parallel constructs constant. Analysis of the scheduling strategy's impact on runtime is presented.

#### 3.4 Experiment 3: Influence of Constructs on Program Runtime

This experiment centers on three different constructs—Reduction Construct, Critical Partial Construct, and Critical Construct—and their influence on program runtime, given other variables remain constant. Detailed analysis and conclusions based on experimental data are provided.

### 4. Conclusion

Key findings regarding the impact of thread count, choice of scheduling strategy, and construct selection are summarized. Practical application value and future prospects for research in parallel computing are discussed.


## Code

### critical1.c

### critical2.c

### reduction.c

### static_dynamic_guided.c



