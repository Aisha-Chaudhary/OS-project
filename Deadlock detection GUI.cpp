#include <windows.h>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

#define ID_MAX 1
#define ID_ALLOC 2
#define ID_AVAIL 3
#define ID_CHECK 4
#define ID_RESULT 5

// Global variables for storing user inputs
vector<vector<int>> maxMatrix;
vector<vector<int>> allocationMatrix;
vector<int> availableResources;
int numProcesses;
int numResources;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndMax, hwndAlloc, hwndAvail, hwndCheck, hwndResult;
    static HINSTANCE hInstance;

    switch (msg) {
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

        CreateWindow("STATIC", "Enter max matrix (row-wise, space separated):", WS_VISIBLE | WS_CHILD, 20, 20, 400, 20, hwnd, NULL, hInstance, NULL);
        hwndMax = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 20, 40, 400, 100, hwnd, (HMENU)ID_MAX, hInstance, NULL);

        CreateWindow("STATIC", "Enter allocation matrix (row-wise, space separated):", WS_VISIBLE | WS_CHILD, 20, 150, 400, 20, hwnd, NULL, hInstance, NULL);
        hwndAlloc = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 20, 170, 400, 100, hwnd, (HMENU)ID_ALLOC, hInstance, NULL);

        CreateWindow("STATIC", "Enter available resources (space separated):", WS_VISIBLE | WS_CHILD, 20, 280, 400, 20, hwnd, NULL, hInstance, NULL);
        hwndAvail = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 300, 400, 20, hwnd, (HMENU)ID_AVAIL, hInstance, NULL);

        hwndCheck = CreateWindow("BUTTON", "Check Safe State", WS_VISIBLE | WS_CHILD, 20, 330, 150, 30, hwnd, (HMENU)ID_CHECK, hInstance, NULL);

        hwndResult = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD, 20, 370, 400, 100, hwnd, (HMENU)ID_RESULT, hInstance, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_CHECK) {
            char buffer[1024];

            // Get max matrix
            GetWindowText(hwndMax, buffer, sizeof(buffer));
            stringstream ssMax(buffer);
            maxMatrix.clear();
            for (int i = 0; i < numProcesses; i++) {
                vector<int> row;
                for (int j = 0; j < numResources; j++) {
                    int val;
                    ssMax >> val;
                    row.push_back(val);
                }
                maxMatrix.push_back(row);
            }

            // Get allocation matrix
            GetWindowText(hwndAlloc, buffer, sizeof(buffer));
            stringstream ssAlloc(buffer);
            allocationMatrix.clear();
            for (int i = 0; i < numProcesses; i++) {
                vector<int> row;
                for (int j = 0; j < numResources; j++) {
                    int val;
                    ssAlloc >> val;
                    row.push_back(val);
                }
                allocationMatrix.push_back(row);
            }

            // Get available resources
            GetWindowText(hwndAvail, buffer, sizeof(buffer));
            stringstream ssAvail(buffer);
            availableResources.clear();
            for (int i = 0; i < numResources; i++) {
                int val;
                ssAvail >> val;
                availableResources.push_back(val);
            }

            // Check if system is in a safe state
            vector<int> work = availableResources;
            vector<bool> finish(numProcesses, false);
            vector<int> safeSequence(numProcesses);

            int count = 0;
            bool safe = true;
            while (count < numProcesses) {
                bool found = false;
                for (int p = 0; p < numProcesses; p++) {
                    if (!finish[p]) {
                        int j;
                        for (j = 0; j < numResources; j++) {
                            if (maxMatrix[p][j] - allocationMatrix[p][j] > work[j])
                                break;
                        }
                        if (j == numResources) {
                            for (int k = 0; k < numResources; k++)
                                work[k] += allocationMatrix[p][k];
                            safeSequence[count++] = p;
                            finish[p] = true;
                            found = true;
                        }
                    }
                }
                if (!found) {
                    safe = false;
                    break;
                }
            }

            string result;
            if (safe) {
                result = "The system is in a safe state.\nSafe sequence is: ";
                for (int i = 0; i < numProcesses; i++) {
                    result += "P" + to_string(safeSequence[i]) + " ";
                }
            } else {
                result = "The system is not in a safe state.";
            }
            SetWindowText(hwndResult, result.c_str());
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    numProcesses = 3; // For demonstration, set this to the number of processes
    numResources = 3; // For demonstration, set this to the number of resources

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "WindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "WindowClass",
        "Banker's Algorithm",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 480, 520,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}




