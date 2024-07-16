import sys


def euclidean_distance(point1, point2):
    return sum((point1[d] - point2[d]) ** 2 for d in range(len(point1))) ** 0.5


error_msg = "An Error Has Occurred"

try:
    k = int(sys.argv[1])
    iter_num = int(sys.argv[2]) if len(sys.argv) >= 4 else 200
    path_to_file = sys.argv[3] if len(sys.argv) >= 4 else sys.argv[2]
    epsilon = 0.001

    if not (1 < iter_num < 1000):
        error_msg = "Invalid maximum iteration!"
        sys.exit(1)

    input_file = open(path_to_file, "r")
    N = len(input_file.readlines())
    input_file.seek(0)

    if not (1 < k < N):
        error_msg = "Invalid number of clusters!"
        sys.exit(1)

    centroids = [
        tuple(float(x) for x in input_file.readline().strip().split(","))
        for _ in range(k)
    ]

    for iter_i in range(int(iter_num)):
        # clear previous clusters
        clusters = [[] for _ in range(k)]
        # reset pointer for file reading
        input_file.seek(0)
        i = 0
        for line in input_file.readlines():
            # iterate over each point and assign to the closest cluster
            point = tuple([float(x) for x in line.split(",")])
            distances = [euclidean_distance(point, centroids[i]) for i in range(k)]
            closest = min(range(k), key=lambda x: distances[x])
            clusters[closest].append(point)
            i += 1

        # calculating new centroids for checking convergence
        new_centroids = [
            tuple(sum(coord) / len(cluster) for coord in zip(*cluster))
            for cluster in clusters
        ]

        # if max convergence for all new centroids is less than epsilon then the condition applies for all
        if (
                max(euclidean_distance(centroids[i], new_centroids[i]) for i in range(k))
                < epsilon
        ):
            centroids = new_centroids
            break

        centroids = new_centroids

    input_file.close()

    for centroid in centroids:
        print(",".join(["%.4f" % coord for coord in centroid]))

except:
    print(error_msg)
    sys.exit(1)
