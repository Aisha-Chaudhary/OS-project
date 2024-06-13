#include <iostream>
#include <vector>

using namespace std;

bool isSafe(vector<vector<int>>& max, vector<vector<int>>& allocation, vector<vector<int>>& need, vector<int>& available) {
    int numProcesses = max.size();
    int numResources = max[0].size();
    vector<bool> finish(numProcesses, false);
    vector<int> work = available;

    // Find an index 'i' such that both finish[i] is false and need[i] <= work
    for (int k = 0; k < numProcesses; ++k) {
        for (int i = 0; i < numProcesses; ++i) {
            if (!finish[i]) {
                bool canAllocate = true;
                for (int j = 0; j < numResources; ++j) {
                    if (need[i][j] > work[j]) {
                        canAllocate = false;
                        break;
                    }
                }
                if (canAllocate) {
                    for (int j = 0; j < numResources; ++j) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    k = -1; // Start over to check all processes again
                }
            }
        }
    }

    // Check if all processes are finished
    for (int i = 0; i < numProcesses; ++i) {
        if (!finish[i]) {
            return false; // System is not in a safe state
        }
    }

    return true; // System is in a safe state
}

void calculateNeed(vector<vector<int>>& need, vector<vector<int>>& max, vector<vector<int>>& allocation) {
    int numProcesses = max.size();
    int numResources = max[0].size();
    for (int i = 0; i < numProcesses; ++i) {
        for (int j = 0; j < numResources; ++j) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
}

int main() {
    int numProcesses, numResources;

    cout << "Enter the number of processes: ";
    cin >> numProcesses;
    cout << "Enter the number of resources: ";
    cin >> numResources;

    vector<vector<int>> max(numProcesses, vector<int>(numResources));
    vector<vector<int>> allocation(numProcesses, vector<int>(numResources));
    vector<vector<int>> need(numProcesses, vector<int>(numResources));
    vector<int> available(numResources);

    cout << "Enter the maximum resource matrix:\n";
    for (int i = 0; i < numProcesses; ++i) {
        for (int j = 0; j < numResources; ++j) {
            cin >> max[i][j];
        }
    }

    cout << "Enter the allocation resource matrix:\n";
    for (int i = 0; i < numProcesses; ++i) {
        for (int j = 0; j < numResources; ++j) {
            cin >> allocation[i][j];
        }
    }

    cout << "Enter the available resources:\n";
    for (int i = 0; i < numResources; ++i) {
        cin >> available[i];
    }

    calculateNeed(need, max, allocation);

    if (isSafe(max, allocation, need, available)) {
        cout << "The system is in a safe state.\n";
    } else {
        cout << "The system is not in a safe state.\n";
    }

    return 0;
}

