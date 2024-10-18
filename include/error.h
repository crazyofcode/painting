#ifndef ERROR_H_
#define ERROR_H_

typedef struct {
    int error_code;
    const char *description;
} ErrorEntry;

// 定义错误码和描述的数组
ErrorEntry error_table[] = {
    {0, "Completed successfully"},
    {-1, "Failed"},
    {-2, "Not supported"},
    {-3, "Invalid parameter(s)"},
    {-4, "Denied or not allowed"},
    {-5, "Invalid address(s)"},
    {-6, "Already available"},
    {-7, "Already started"},
    {-8, "Already stopped"},
    {-9, "Shared memory not available"},
    {-10, "Invalid state"},
    {-11, "Bad (or invalid) range"},
    {-12, "Failed due to timeout"},
    {-13, "Input/Output error"}
};

#endif //!ERROR_H_
