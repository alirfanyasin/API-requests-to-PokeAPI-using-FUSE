#define FUSE_USE_VERSION 28
#include <stdio.h>
#include "requests.h"
#include <string.h>
#include "json.h"
#include <sys/stat.h>
#include <fuse.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/bytecode/Documents/pokemon";
char path[100];

void request(req_t *req, const char argv[30]){
    int ret = requests_init(req);
    if (ret) {
        return;
    }
    char string[1000] = "https://pokeapi.co/api/v2/pokemon/";
    strcat(string, argv);
    requests_get(req, string);
}

static void process_value(json_value* value, int depth)
{
    FILE *file;
    char tmp[200];
    char filePath[300];

    //Profile Section
    snprintf(filePath, sizeof(filePath),"%s/%s.txt", path, value->u.object.values[10].value->u.string.ptr);
    file = fopen(filePath, "w");
    fprintf(file, "%s : %d\n", value->u.object.values[6].name, (int)value->u.object.values[6].value->u.integer);
    fprintf(file, "%s : %s\n", value->u.object.values[10].name, value->u.object.values[10].value->u.string.ptr);
    fprintf(file, "%s : %d\n", value->u.object.values[1].name, (int)value->u.object.values[1].value->u.integer);
    fprintf(file, "%s : %d\n", value->u.object.values[11].name, (int)value->u.object.values[11].value->u.integer);
    fprintf(file, "%s : %d\n", value->u.object.values[4].name, (int)value->u.object.values[4].value->u.integer);
    fprintf(file, "%s : %d\n", value->u.object.values[17].name, (int)value->u.object.values[17].value->u.integer);
    if (value->u.object.values[10].value->u.boolean)
    {
        fprintf(file, "%s : %s\n", value->u.object.values[7].name, "True");
    }else
    {
        fprintf(file, "%s : %s\n", value->u.object.values[7].name, "False");
    }
    fprintf(file, "%s : %s\n", value->u.object.values[8].name, value->u.object.values[8].value->u.string.ptr);
    fclose(file);
    //End Profile

    //Abilities Section
    snprintf(tmp, sizeof(tmp),"%s/%s", path, value->u.object.values[0].name);
    mkdir(tmp, 0777);
    for (int i = 0; i < value->u.object.values[0].value->u.array.length; i++)
    {
        snprintf(filePath, sizeof(filePath),"%s/%s.txt", tmp, value->u.object.values[0].value->u.array.values[i]->u.object.values[0].value->u.object.values[0].value->u.string.ptr);
        file = fopen(filePath, "w");
        for (int j = 0; j < value->u.object.values[0].value->u.array.values[i]->u.object.values[0].value->u.object.length; j++)
        {
            fprintf(file, "%s : %s\n", value->u.object.values[0].value->u.array.values[i]->u.object.values[0].value->u.object.values[j].name, value->u.object.values[0].value->u.array.values[i]->u.object.values[0].value->u.object.values[j].value->u.string.ptr);
        }
        for (int j = 1; j < value->u.object.values[0].value->u.array.values[i]->u.object.length; j++)
        {
            if (value->u.object.values[0].value->u.array.values[i]->u.object.values[j].value->type == json_integer)
            {
                fprintf(file, "%s : %d\n", value->u.object.values[0].value->u.array.values[i]->u.object.values[j].name, (int)value->u.object.values[0].value->u.array.values[i]->u.object.values[j].value->u.integer);
            }else if(value->u.object.values[0].value->u.array.values[i]->u.object.values[j].value->type == json_boolean)
            {
                if (value->u.object.values[0].value->u.array.values[i]->u.object.values[j].value->u.boolean)
                {
                    fprintf(file, "%s : %s\n", value->u.object.values[0].value->u.array.values[i]->u.object.values[j].name, "True");
                }else
                {
                    fprintf(file, "%s : %s\n", value->u.object.values[0].value->u.array.values[i]->u.object.values[j].name, "False");
                }
            }
        }
        fclose(file);
    }
    //End Abilities 

    //Game Indices Section
    snprintf(tmp, sizeof(tmp),"%s/%s", path, value->u.object.values[3].name);
    mkdir(tmp, 0777);
    for (int i = 0; i < value->u.object.values[3].value->u.array.length; i++)
    {
        snprintf(filePath, sizeof(filePath),"%s/%s.txt", tmp, value->u.object.values[3].value->u.array.values[i]->u.object.values[1].value->u.object.values[0].value->u.string.ptr);
        file = fopen(filePath, "w");
        fprintf(file, "%s : %d\n", value->u.object.values[3].value->u.array.values[i]->u.object.values[0].name, (int)value->u.object.values[3].value->u.array.values[i]->u.object.values[0].value->u.integer);
        fprintf(file, "%s :\n", value->u.object.values[3].value->u.array.values[i]->u.object.values[1].name);
        fprintf(file, "  %s : %s\n", value->u.object.values[3].value->u.array.values[i]->u.object.values[1].value->u.object.values[0].name, value->u.object.values[3].value->u.array.values[i]->u.object.values[1].value->u.object.values[0].value->u.string.ptr);
        fprintf(file, "  %s : %s\n", value->u.object.values[3].value->u.array.values[i]->u.object.values[1].value->u.object.values[1].name, value->u.object.values[3].value->u.array.values[i]->u.object.values[1].value->u.object.values[1].value->u.string.ptr);
        fclose(file);
    }
    //End Game Indices

    //Held Item Section
    snprintf(tmp, sizeof(tmp),"%s/%s", path, value->u.object.values[5].name);
    mkdir(tmp, 0777);
    for (int i = 0; i < value->u.object.values[5].value->u.array.length; i++)
    {
        snprintf(filePath, sizeof(filePath),"%s/%s.txt", tmp, value->u.object.values[5].value->u.array.values[i]->u.object.values[0].value->u.object.values[0].value->u.string.ptr);
        file = fopen(filePath, "w");
        for (int j = 0; j < value->u.object.values[5].value->u.array.values[i]->u.object.values[0].value->u.object.length; j++)
        {
            fprintf(file, "%s : %s\n", value->u.object.values[5].value->u.array.values[i]->u.object.values[0].value->u.object.values[j].name, value->u.object.values[5].value->u.array.values[i]->u.object.values[0].value->u.object.values[j].value->u.string.ptr);
        }
        fprintf(file, "%s :\n", value->u.object.values[5].value->u.array.values[i]->u.object.values[1].name);
        for (int k = 0; k < value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.length; k++)
        {
            fprintf(file, "\n{\n  %s : %d\n", value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.values[k]->u.object.values[0].name, (int)value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.values[k]->u.object.values[0].value->u.integer);
            fprintf(file, "  %s : \n", value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.values[k]->u.object.values[1].name);
            fprintf(file, "    %s : %s\n", value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.values[k]->u.object.values[1].value->u.object.values[0].name, value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.values[k]->u.object.values[1].value->u.object.values[0].value->u.string.ptr);
            fprintf(file, "    %s : %s\n}\n", value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.values[k]->u.object.values[1].value->u.object.values[1].name, value->u.object.values[5].value->u.array.values[i]->u.object.values[1].value->u.array.values[k]->u.object.values[1].value->u.object.values[1].value->u.string.ptr);
        }
            
        fclose(file);
    }
    //End Held Item
    
    //Moves Section 
    snprintf(tmp, sizeof(tmp),"%s/%s", path, value->u.object.values[9].name);
    mkdir(tmp, 0777);
    for (int i = 0; i < value->u.object.values[9].value->u.array.length; i++)
    {
        snprintf(filePath, sizeof(filePath),"%s/%s.txt", tmp, value->u.object.values[9].value->u.array.values[i]->u.object.values[0].value->u.object.values[0].value->u.string.ptr);
        file = fopen(filePath, "w");
        for (int j = 0; j < value->u.object.values[9].value->u.array.values[i]->u.object.values[0].value->u.object.length; j++)
        {
            fprintf(file, "%s : %s\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[0].value->u.object.values[j].name, value->u.object.values[9].value->u.array.values[i]->u.object.values[0].value->u.object.values[j].value->u.string.ptr);
        }
        for (int j = 0; j < value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.object.length; j++)
        {
            fprintf(file, "\n{\n  %s : %d\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[0].name, (int)value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[0].value->u.integer);
            fprintf(file, "  %s :\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[1].name);
            fprintf(file, "    %s : %s\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[1].value->u.object.values[0].name, value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[1].value->u.object.values[0].value->u.string.ptr);
            fprintf(file, "    %s : %s\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[1].value->u.object.values[1].name, value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[1].value->u.object.values[1].value->u.string.ptr);
            fprintf(file, "  %s :\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[2].name);
            fprintf(file, "    %s : %s\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[2].value->u.object.values[0].name, value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[2].value->u.object.values[0].value->u.string.ptr);
            fprintf(file, "    %s : %s\n}\n", value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[2].value->u.object.values[1].name, value->u.object.values[9].value->u.array.values[i]->u.object.values[1].value->u.array.values[j]->u.object.values[2].value->u.object.values[1].value->u.string.ptr);
        }
        fclose(file);
    }
    //End Moves

    //Sprites Section
    
    //End Section
    snprintf(tmp, sizeof(tmp),"%s/%s", path, value->u.object.values[14].name);
    mkdir(tmp, 0777);
    for (int i = 0; i < value->u.object.values[14].value->u.object.length-2; i++)
    {
        snprintf(filePath, sizeof(filePath),"%s/%s.png", tmp, value->u.object.values[14].value->u.object.values[i].name);
        file = fopen(filePath, "w");
        // fprintf(file, "%s", value->u.object.values[14].value->u.object.values[i].value->u.string.ptr);
        fclose(file);
    }
    //Stats Section
    snprintf(tmp, sizeof(tmp),"%s/%s", path, value->u.object.values[15].name);
    mkdir(tmp, 0777);
    for (int i = 0; i < value->u.object.values[15].value->u.array.length; i++)
    {
        snprintf(filePath, sizeof(filePath),"%s/%s.txt", tmp, value->u.object.values[15].value->u.array.values[i]->u.object.values[2].value->u.object.values[0].value->u.string.ptr);
        file = fopen(filePath, "w");
        fprintf(file, "%s : %s\n",value->u.object.values[15].value->u.array.values[i]->u.object.values[2].value->u.object.values[0].name, value->u.object.values[15].value->u.array.values[i]->u.object.values[2].value->u.object.values[0].value->u.string.ptr);
        fprintf(file, "%s : %d\n",value->u.object.values[15].value->u.array.values[i]->u.object.values[0].name, (int)value->u.object.values[15].value->u.array.values[i]->u.object.values[0].value->u.integer);
        fprintf(file, "%s : %d\n",value->u.object.values[15].value->u.array.values[i]->u.object.values[1].name, (int)value->u.object.values[15].value->u.array.values[i]->u.object.values[1].value->u.integer);
        fprintf(file, "%s : %s\n",value->u.object.values[15].value->u.array.values[i]->u.object.values[2].value->u.object.values[1].name, value->u.object.values[15].value->u.array.values[i]->u.object.values[2].value->u.object.values[1].value->u.string.ptr);

        fclose(file);
    }
    //End Stats

    //Types Section
    snprintf(tmp, sizeof(tmp),"%s/%s", path, value->u.object.values[16].name);
    mkdir(tmp, 0777);
    for (int i = 0; i < value->u.object.values[16].value->u.array.length; i++)
    {
        snprintf(filePath, sizeof(filePath),"%s/%s.txt", tmp, value->u.object.values[16].value->u.array.values[i]->u.object.values[1].value->u.object.values[0].value->u.string.ptr);
        file = fopen(filePath, "w");
        fprintf(file, "%s : %s\n",value->u.object.values[16].value->u.array.values[i]->u.object.values[1].value->u.object.values[0].name, value->u.object.values[16].value->u.array.values[i]->u.object.values[1].value->u.object.values[0].value->u.string.ptr);
        fprintf(file, "%s : %d\n",value->u.object.values[16].value->u.array.values[i]->u.object.values[0].name, (int)value->u.object.values[16].value->u.array.values[i]->u.object.values[0].value->u.integer);
        fprintf(file, "%s : %s\n",value->u.object.values[16].value->u.array.values[i]->u.object.values[1].value->u.object.values[1].name, value->u.object.values[16].value->u.array.values[i]->u.object.values[1].value->u.object.values[1].value->u.string.ptr);
        fclose(file);
    }
    //End Types
}

void jsonParser(req_t req){
    json_value *parsed_json = json_parse(req.text, strlen(req.text));
    process_value(parsed_json, 0);
    json_value_free(parsed_json);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath, "%s%s", dirpath, path);

    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", dirpath, path);

    int res = 0;
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));
        if (res != 0)
            break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", dirpath, path);

    int res = 0;
    int fd = 0;

    (void)fi;

    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    char fpath[1000];
    if(strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    int res = 0;

    res = mkdir(fpath, mode);

    if (res == -1) return -errno;

    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
};


int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <pokemon_name> <folder_name>\n", argv[0]);
        return 1;
    }
    
    req_t req;
    request(&req, argv[1]);

    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    snprintf(path, sizeof(path),"%s/%s", dirpath, argv[1]);
    mkdir(path, 0777);
//     printf("Response Body:\n%s\n", req.text);
    jsonParser(req);

    requests_close(&req); /* clean up */
    umask(0);
    return fuse_main(argc-1, argv+1, &xmp_oper, NULL);
}
