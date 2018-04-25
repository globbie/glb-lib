#include <glb-lib/json_converter.h>
#include <glb-lib/output.h>

#include <stdio.h>
#include <stdlib.h>

int main (int argc __attribute__((unused)), const char *argv[])
{
    int exit_code = EXIT_FAILURE;
    struct glbOutput *input = NULL, *output = NULL;

    if (glb_OK != glbOutput_new(&input, 1e6)) goto final;
    if (glb_OK != input->write_file_content(input, argv[1])) goto final;

    if (glb_OK != glbOutput_new(&output, 1e6)) goto final;

    if (glb_OK != glb_json_to_gsl(input->buf, input->buf_size, NULL, output)) goto final;

    printf("%.*s\n\n", (int)output->buf_size, output->buf);

    exit_code = EXIT_SUCCESS;

final:
    if (input != NULL)
        input->del(input);
    if (output != NULL)
        output->del(output);
    return exit_code;
}

