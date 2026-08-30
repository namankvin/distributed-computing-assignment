# Matrix Transpose using Hadoop MapReduce

This repository contains the Docker/Hadoop environment and all work required up to **Milestone 5** of the Matrix Transpose using Hadoop MapReduce assignment.

This README is written for a teammate starting from a fresh **Windows 10/11** machine.

After completing the steps below, you will have:

- Docker Desktop running with WSL2
- Ubuntu through WSL2
- 1 Hadoop NameNode
- 2 Hadoop DataNodes
- HDFS working
- Hadoop Web UIs working
- YARN ResourceManager running
- YARN NodeManager running on both DataNodes
- Matrix generator compiled
- 16,384 × 16,384 integer matrix generated
- Format A generated
- Format B generated as 64 block files
- Both formats uploaded to HDFS
- HDFS storage usage verified
- Environment ready for **Milestone 6**

---

# 1. Project Structure

After cloning, the repository should look approximately like:

```text
hadoop-cluster/
│
├── config/                 # Hadoop configuration files
├── docker-compose.yml      # Creates the Hadoop cluster
├── start-hdfs.sh           # Starts the HDFS NameNode
├── init-datanode.sh        # Starts DataNodes while preserving HDFS blocks
│
├── generate_matrix.cpp     # Milestone 5 matrix generator
├── input.txt               # Previous WordCount input
│
├── .gitignore
├── .gitattributes
└── README.md
```

The following are intentionally **not stored in GitHub**:

```text
format_a/
format_b/

namenode/
datanode1/
datanode2/

generate_matrix
generate_matrix.exe
```

They are generated locally.

---

# 2. Windows Setup

Use:

- Windows 10/11
- WSL2
- Ubuntu
- Docker Desktop
- Git
- g++

Use the **Ubuntu/WSL terminal** for project commands unless this README specifically says to use PowerShell.

---

# 3. Install WSL2 and Ubuntu

Open **PowerShell as Administrator**.

Run:

```powershell
wsl --install -d Ubuntu
```

Restart Windows if requested.

After restarting, open:

```text
Ubuntu
```

from the Start Menu.

Ubuntu will ask you to create:

```text
username
password
```

Complete that setup.

---

# 4. Install Docker Desktop

Open normal PowerShell and run:

```powershell
winget install -e --id Docker.DockerDesktop
```

After installation, open Docker Desktop.

Go to:

```text
Settings
→ General
```

Make sure:

```text
Use the WSL 2 based engine
```

is enabled.

Then go to:

```text
Settings
→ Resources
→ WSL Integration
```

Enable Docker integration for:

```text
Ubuntu
```

Apply/restart Docker Desktop if requested.

Leave Docker Desktop running.

---

# 5. Install Required Packages in Ubuntu

Open the Ubuntu terminal.

Run:

```bash
sudo apt update
```

Then:

```bash
sudo apt install -y git build-essential
```

Verify Git:

```bash
git --version
```

Verify C++:

```bash
g++ --version
```

Verify Docker:

```bash
docker --version
```

Verify Docker Compose:

```bash
docker compose version
```

All four should print version information.

If Docker commands fail inside Ubuntu:

```text
Docker Desktop
→ Settings
→ Resources
→ WSL Integration
```

and make sure Ubuntu is enabled.

---

# 6. Recommended Disk Space

Milestone 5 generates several GB of data.

Use a machine with at least:

```text
30–40 GB free disk space
```

More is preferable.

---

# 7. Clone the Repository

Inside Ubuntu:

```bash
cd ~
```

Create a projects directory:

```bash
mkdir -p projects
cd projects
```

Clone:

```bash
git clone <REPOSITORY-URL>
```

Replace `<REPOSITORY-URL>` with this project's GitHub clone URL.

Enter the repository:

```bash
cd hadoop-cluster
```

Check:

```bash
ls
```

You should see:

```text
config
docker-compose.yml
generate_matrix.cpp
init-datanode.sh
input.txt
start-hdfs.sh
README.md
```

---

# 8. Keep the Project inside WSL

Keep the repository somewhere similar to:

```text
/home/<username>/projects/hadoop-cluster
```

Avoid placing it inside:

```text
/mnt/c/
```

when possible.

Working inside the WSL filesystem is faster for the large amount of file I/O used in this assignment.

---

# 9. Validate Docker Compose

Make sure you are inside:

```text
hadoop-cluster
```

Run:

```bash
docker compose config
```

If no YAML/configuration error appears, continue.

---

# 10. Start the Hadoop Cluster

Run:

```bash
docker compose up -d
```

`-d` means detached mode.

The first run may download:

```text
apache/hadoop:3.4.1
```

and can take some time.

Wait approximately 10–20 seconds after the containers start.

---

# 11. Verify the Containers

Run:

```bash
docker ps
```

You should see:

```text
namenode
datanode1
datanode2
```

All three should show a status similar to:

```text
Up ...
```

The architecture is:

```text
                    NameNode
                       |
               -----------------
               |               |
           DataNode1       DataNode2
```

The NameNode manages HDFS metadata.

The DataNodes store actual HDFS blocks.

---

# 12. Verify Hadoop Web Interfaces

Open the following URLs in your normal Windows browser.

## NameNode

```text
http://localhost:9870
```

The NameNode web UI should open.

It should eventually show:

```text
Live Nodes: 2
```

## DataNode 1

```text
http://localhost:9864
```

## DataNode 2

```text
http://localhost:9865
```

Both DataNodes internally use Hadoop port `9864`.

Docker maps:

```text
Host Port     Container

9864       →  DataNode1:9864
9865       →  DataNode2:9864
```

---

# 13. HDFS Persistence

The Hadoop storage directories are bind-mounted from the host:

```text
./namenode
./datanode1
./datanode2
```

The DataNode startup script has been modified so that existing HDFS block data is **preserved across container recreation**.

Therefore:

```bash
docker compose down
docker compose up -d
```

should preserve existing HDFS data.

The containers themselves are recreated, but the HDFS files remain in the host directories.

YARN processes do **not** persist and must be started again after container recreation.

To intentionally create a completely fresh HDFS cluster, see the reset section near the bottom of this README.

---

# 14. Start YARN

The supplied Docker startup scripts start HDFS but do not automatically start YARN.

MapReduce requires YARN.

The required structure is:

```text
namenode
├── HDFS NameNode
└── YARN ResourceManager

datanode1
├── HDFS DataNode
└── YARN NodeManager

datanode2
├── HDFS DataNode
└── YARN NodeManager
```

From the Ubuntu terminal, start the ResourceManager:

```bash
docker exec -d namenode yarn resourcemanager
```

Start DataNode1's NodeManager:

```bash
docker exec -d datanode1 yarn nodemanager
```

Start DataNode2's NodeManager:

```bash
docker exec -d datanode2 yarn nodemanager
```

---

# 15. Verify YARN

Check ResourceManager:

```bash
docker exec namenode ps aux | grep ResourceManager
```

You should see a Java command containing:

```text
org.apache.hadoop.yarn.server.resourcemanager.ResourceManager
```

Check DataNode1:

```bash
docker exec datanode1 ps aux | grep NodeManager
```

Check DataNode2:

```bash
docker exec datanode2 ps aux | grep NodeManager
```

Both should show Java NodeManager processes.

`jps` is not required and may not be available in this Hadoop image.

---

# 16. Milestone 5 Matrix

Milestone 5 uses a:

```text
16384 × 16384
```

integer matrix.

A 32-bit integer requires:

```text
4 bytes
```

1 GiB is:

```text
1,073,741,824 bytes
```

Therefore:

```text
1,073,741,824 / 4
= 268,435,456 integers
```

Since:

```text
16384 × 16384
= 268,435,456
```

the chosen square matrix dimension is:

```text
16384 × 16384
```

---

# 17. Storage Formats

## Format A

Every matrix value is stored as:

```text
row,column,value
```

Example:

```text
0,0,25
0,1,42
0,2,19
1,0,72
```

The generated file is:

```text
format_a/matrix_entries.txt
```

---

## Format B

The matrix is divided into:

```text
2048 × 2048
```

matrix blocks.

Since:

```text
16384 / 2048 = 8
```

there are:

```text
8 × 8
= 64 block files
```

The files are named:

```text
block_0_0.txt
block_0_1.txt
...
block_7_7.txt
```

and stored in:

```text
format_b/
```

The block filename identifies that block's row and column.

Milestone 6 uses this information inside the Hadoop Mapper.

---

# 18. Compile the Matrix Generator

From the repository root:

```bash
g++ -std=c++17 -O2 generate_matrix.cpp -o generate_matrix
```

Verify the executable exists:

```bash
ls
```

You should see:

```text
generate_matrix
```

---

# 19. Test the Generator

Always test using a tiny matrix first.

Run:

```bash
./generate_matrix 8 4
```

This creates:

```text
format_a/
format_b/
```

Check Format A:

```bash
head format_a/matrix_entries.txt
```

Check the first block:

```bash
cat format_b/block_0_0.txt
```

For:

```text
8 × 8 matrix
4 × 4 blocks
```

there should be:

```text
2 × 2
= 4 block files
```

---

# 20. Delete the Test Dataset

Run:

```bash
rm -rf format_a format_b
```

This removes only the generated local test files.

---

# 21. Generate the Full Matrix

Run:

```bash
./generate_matrix 16384 2048
```

This may take significant time.

Do not interrupt it unnecessarily.

When complete:

```text
format_a/
└── matrix_entries.txt

format_b/
├── block_0_0.txt
├── block_0_1.txt
├── ...
└── block_7_7.txt
```

---

# 22. Check Local Sizes

Run:

```bash
du -sh format_a
```

Then:

```bash
du -sh format_b
```

These show local Linux filesystem usage.

They are not HDFS measurements.

---

# 23. Copy the Data into the NameNode Container

Copy Format A:

```bash
docker cp format_a namenode:/tmp/format_a
```

Copy Format B:

```bash
docker cp format_b namenode:/tmp/format_b
```

The files now exist inside the NameNode container but are still **not inside HDFS**.

---

# 24. Enter the NameNode Container

Run:

```bash
docker exec -it namenode bash
```

Your prompt should become similar to:

```text
[root@namenode hadoop]#
```

Commands after this point are being executed inside the NameNode container until `exit` is used.

---

# 25. Create the HDFS Directories

Run:

```bash
hdfs dfs -mkdir /matrix_data
```

Then:

```bash
hdfs dfs -mkdir /matrix_data/format_a
```

Then:

```bash
hdfs dfs -mkdir /matrix_data/format_b
```

HDFS should conceptually contain:

```text
/
└── matrix_data/
    ├── format_a/
    └── format_b/
```

---

# 26. Upload Format A into HDFS

Run:

```bash
hdfs dfs -put /tmp/format_a/matrix_entries.txt /matrix_data/format_a
```

---

# 27. Upload Format B into HDFS

Run:

```bash
hdfs dfs -put /tmp/format_b/* /matrix_data/format_b
```

This uploads the 64 block files.

---

# 28. Verify Format A

Run:

```bash
hdfs dfs -ls /matrix_data/format_a
```

You should see:

```text
/matrix_data/format_a/matrix_entries.txt
```

---

# 29. Verify Format B

Run:

```bash
hdfs dfs -ls /matrix_data/format_b
```

You should see files such as:

```text
block_0_0.txt
block_0_1.txt
...
block_7_7.txt
```

---

# 30. Measure HDFS Storage Usage

Run:

```bash
hdfs dfs -du -s -h /matrix_data/format_a
```

Then:

```bash
hdfs dfs -du -s -h /matrix_data/format_b
```

`du` means:

```text
Disk Usage
```

Options:

```text
-s = summary
-h = human-readable
```

Hadoop may display two sizes.

Example:

```text
4.0 G    8.0 G    /matrix_data/format_a
```

The first value is logical size.

The second is actual HDFS space consumed after replication.

The current cluster uses:

```text
Replication Factor = 2
```

so actual HDFS storage is approximately twice the logical size.

---

# 31. Verify HDFS Health

Run:

```bash
hdfs fsck /matrix_data/format_b -files -blocks -locations
```

Near the end, the desired result is:

```text
HEALTHY
```

You can also check Format A:

```bash
hdfs fsck /matrix_data/format_a -files -blocks -locations
```

---

# 32. Exit the NameNode

Run:

```bash
exit
```

You should return to the Ubuntu terminal.

---

# 33. Milestone 5 Complete

At this point:

```text
Docker Hadoop cluster       ✓
NameNode                    ✓
2 DataNodes                 ✓
HDFS                        ✓

YARN ResourceManager        ✓
NodeManager on DataNode1    ✓
NodeManager on DataNode2    ✓

16384 × 16384 matrix        ✓
Format A generated          ✓
Format B generated          ✓
64 block files              ✓

Format A uploaded to HDFS   ✓
Format B uploaded to HDFS   ✓
HDFS storage measured       ✓
HDFS health verified        ✓
```

---

# 34. Final Check Before Milestone 6

From Ubuntu:

```bash
docker ps
```

You should see:

```text
namenode
datanode1
datanode2
```

Verify ResourceManager:

```bash
docker exec namenode ps aux | grep ResourceManager
```

Verify DataNode1 NodeManager:

```bash
docker exec datanode1 ps aux | grep NodeManager
```

Verify DataNode2 NodeManager:

```bash
docker exec datanode2 ps aux | grep NodeManager
```

Enter the NameNode:

```bash
docker exec -it namenode bash
```

Check Format B:

```bash
hdfs dfs -ls /matrix_data/format_b
```

Check health:

```bash
hdfs fsck /matrix_data/format_b -files -blocks -locations
```

Only continue when it reports:

```text
HEALTHY
```

Exit:

```bash
exit
```

You are ready for:

# Milestone 6 — Block-Based Matrix Transpose using Hadoop MapReduce

Use:

```text
/matrix_data/format_b
```

as the MapReduce input.

---

# 35. Restarting the Cluster

HDFS data is configured to persist across Docker container recreation.

To stop the cluster:

```bash
docker compose down
```

To start it again:

```bash
docker compose up -d
```

Wait approximately 10–20 seconds.

Verify:

```bash
docker ps
```

Your existing HDFS data should still be available because:

```text
namenode/
datanode1/
datanode2/
```

remain on the host and are bind-mounted back into the new containers.

---

# 36. After Every Docker Restart

YARN processes must be started again.

Run:

```bash
docker exec -d namenode yarn resourcemanager
```

```bash
docker exec -d datanode1 yarn nodemanager
```

```bash
docker exec -d datanode2 yarn nodemanager
```

Verify:

```bash
docker exec namenode ps aux | grep ResourceManager
```

```bash
docker exec datanode1 ps aux | grep NodeManager
```

```bash
docker exec datanode2 ps aux | grep NodeManager
```

---

# 37. Verify HDFS Data After Restart

Enter the NameNode:

```bash
docker exec -it namenode bash
```

Run:

```bash
hdfs dfs -ls /matrix_data/format_b
```

Then:

```bash
hdfs fsck /matrix_data/format_b -files -blocks -locations
```

The expected result is:

```text
HEALTHY
```

Exit:

```bash
exit
```

---

# 38. Completely Reset HDFS

Only use this if you intentionally want to destroy all current HDFS data and create a fresh cluster.

Stop Docker Compose:

```bash
docker compose down
```

Delete all Hadoop runtime data:

```bash
rm -rf namenode datanode1 datanode2
```

Start the cluster again:

```bash
docker compose up -d
```

The NameNode will be formatted again because no previous NameNode metadata exists.

Start YARN:

```bash
docker exec -d namenode yarn resourcemanager
docker exec -d datanode1 yarn nodemanager
docker exec -d datanode2 yarn nodemanager
```

If the local:

```text
format_a/
format_b/
```

directories still exist, the matrix does **not** need to be regenerated.

Copy them into the NameNode again:

```bash
docker cp format_a namenode:/tmp/format_a
docker cp format_b namenode:/tmp/format_b
```

Enter NameNode:

```bash
docker exec -it namenode bash
```

Recreate HDFS directories:

```bash
hdfs dfs -mkdir /matrix_data
hdfs dfs -mkdir /matrix_data/format_a
hdfs dfs -mkdir /matrix_data/format_b
```

Upload Format A:

```bash
hdfs dfs -put /tmp/format_a/matrix_entries.txt /matrix_data/format_a
```

Upload Format B:

```bash
hdfs dfs -put /tmp/format_b/* /matrix_data/format_b
```

Verify:

```bash
hdfs fsck /matrix_data/format_b -files -blocks -locations
```

Continue only when the filesystem reports:

```text
HEALTHY
```

Exit:

```bash
exit
```

---

# 39. Useful Docker Commands

Show running containers:

```bash
docker ps
```

Validate Compose:

```bash
docker compose config
```

Start cluster:

```bash
docker compose up -d
```

Stop/remove Compose containers:

```bash
docker compose down
```

Enter NameNode:

```bash
docker exec -it namenode bash
```

Copy host files into a container:

```bash
docker cp <source> <container>:<destination>
```

---

# 40. Useful HDFS Commands

List HDFS files:

```bash
hdfs dfs -ls <path>
```

Create an HDFS directory:

```bash
hdfs dfs -mkdir <path>
```

Upload a file into HDFS:

```bash
hdfs dfs -put <local-path> <hdfs-path>
```

Display an HDFS file:

```bash
hdfs dfs -cat <hdfs-file>
```

Delete an HDFS file:

```bash
hdfs dfs -rm <hdfs-file>
```

Delete an HDFS directory recursively:

```bash
hdfs dfs -rm -r <hdfs-directory>
```

Show HDFS storage usage:

```bash
hdfs dfs -du -s -h <hdfs-path>
```

Check HDFS health:

```bash
hdfs fsck <path> -files -blocks -locations
```

---

# 41. Useful YARN Commands

Start ResourceManager:

```bash
docker exec -d namenode yarn resourcemanager
```

Start NodeManagers:

```bash
docker exec -d datanode1 yarn nodemanager
docker exec -d datanode2 yarn nodemanager
```

Verify:

```bash
docker exec namenode ps aux | grep ResourceManager
docker exec datanode1 ps aux | grep NodeManager
docker exec datanode2 ps aux | grep NodeManager
```

---

# 42. Git Rules

Do not commit generated matrix files or Hadoop runtime data.

The repository `.gitignore` excludes:

```text
format_a/
format_b/

namenode/
datanode1/
datanode2/

generate_matrix
generate_matrix.exe

*.class
build/
target/

.vscode/
.idea/
.DS_Store
```

Commit source/configuration files such as:

```text
generate_matrix.cpp
docker-compose.yml
config/
init-datanode.sh
start-hdfs.sh
README.md
*.java
```

Do not commit:

```text
format_a/
format_b/
namenode/
datanode1/
datanode2/
generate_matrix
```

---

# 43. Ready for Milestone 6

Before starting Milestone 6, confirm:

```text
[✓] Docker Desktop is running
[✓] namenode is running
[✓] datanode1 is running
[✓] datanode2 is running
[✓] NameNode reports 2 live DataNodes
[✓] ResourceManager is running
[✓] Both NodeManagers are running
[✓] /matrix_data/format_b exists
[✓] 64 block files exist
[✓] HDFS reports HEALTHY
```

If all checks pass, begin implementing the Hadoop MapReduce matrix transpose for **Milestone 6**.