# Master's Thesis Supplement
**G(enetic)A(lgorithm)B(ack)P(ropagation)**

This repository contains supplementary materials for a Master's Thesis titled:

**Projektowanie struktury sieci neuronowej za pomocą wyspowego równoległego algorytmu genetycznego**

Please note that there is a limitted documentation on how to use this code.

# Building and running Docker container

In order to run the code, use Docker. This is the simplest way to do it. You can also build everything manually.

```
git clone https://github.com/mkowsiak/gabp
cd gabp
docker build . -t gabp
docker run gabp
```

# Layout of the directory

```
.
|-- src                                    - source code
|-- bin                                    - location of main file
|-- config                                 - configuration files
|-- test                                   - location of learn/test
|                                            files
`-- run.sh                                 - Docker entry point
```

# Requirements

All you need is `Git` and `Docker`.

# Note

Please note this code is more than 20 years old. Still compiles and run though ;) It was not maintained since.
