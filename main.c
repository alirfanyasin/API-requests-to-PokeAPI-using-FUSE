// menentukan versi FUSE yang akan digunakan dalam program. Dalam hal ini, versi FUSE yang digunakan adalah versi 28.
#define FUSE_USE_VERSION 28 
// header-file standar C yang diperlukan untuk fungsi input/output standar.
#include <stdio.h>
// header-file yang mengandung definisi untuk fungsi-fungsi yang berkaitan dengan permintaan HTTP, seperti yang digunakan dalam pustaka
#include "requests.h"
// eader-file standar C yang diperlukan untuk operasi terkait string,
#include <string.h>
// header-file yang mengandung definisi untuk operasi penguraian JSON, seperti yang digunakan dalam fungsi json_parse dan json_value.
#include "json.h"
// Ini adalah inklusi dari header-file sistem yang diperlukan untuk operasi berkas dan direktori,
#include <sys/stat.h>
// header-file yang berkaitan dengan FUSE (Filesystem in Userspace), yang diperlukan untuk implementasi sistem berkas dalam program.
#include <fuse.h>
// header-file sistem yang diperlukan untuk operasi sistem, 
#include <unistd.h>
// header-file sistem yang diperlukan untuk operasi berkas,
#include <fcntl.h>
// header-file sistem yang diperlukan untuk operasi direktori,
#include <dirent.h>
// header-file sistem yang mengandung definisi kode kesalahan, yang akan digunakan untuk menangani kesalahan saat operasi berkas dan sistem.
#include <errno.h>
// eader-file sistem yang diperlukan untuk operasi terkait waktu, 
#include <sys/time.h>


/*
Ini adalah deklarasi sebuah variabel global konstan dirpath yang berisi jalur (path) ke direktori tempat struktur berkas akan dibuat. 
Ini merupakan direktori utama yang akan digunakan dalam program
*/
static const char *dirpath = "/home/bytecode/Documents/pokemon";
char path[100];


/*
definisi fungsi request yang memiliki dua parameter: sebuah pointer ke tipe data req_t (yang digunakan untuk permintaan HTTP) 
dan sebuah array karakter argv dengan panjang maksimum 30. Fungsi ini bertujuan untuk melakukan permintaan ke API Pokemon 
menggunakan pustaka librequests dan mendapatkan data tentang pokemon.
*/
void request(req_t *req, const char argv[30]){
    int ret = requests_init(req);
    if (ret) {
        return;
    }
    char string[1000] = "https://pokeapi.co/api/v2/pokemon/";
    strcat(string, argv);
    requests_get(req, string);
}


/*

Fungsi process_value pada kode di atas merupakan fungsi yang bertanggung jawab untuk memproses dan menyimpan data dari objek JSON
yang diberikan ke dalam berkas teks (.txt). Fungsi ini menerima dua parameter: json_value* value yang merupakan pointer ke struktur
data JSON yang akan diproses, dan int depth yang mengindikasikan kedalaman proses.
*/
static void process_value(json_value* value, int depth)
{
    FILE *file;
    char tmp[200];
    char filePath[300];

    //Profile Section
    /*
    Bagian ini memproses dan menyimpan data profil pokemon ke dalam berkas teks. Informasi seperti nama, tinggi, berat,
    base experience, dan lain-lain disimpan dalam berkas teks yang memiliki nama sesuai dengan nama pokemon.
    */
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
    /*
    Bagian ini memproses dan menyimpan data kemampuan (abilities) pokemon ke dalam direktori yang bernama sesuai
    dengan nama pokemon. Setiap kemampuan disimpan dalam berkas teks dengan nama yang sesuai.
    */
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
    /*
    Bagian ini memproses dan menyimpan data game indices pokemon ke dalam direktori yang bernama sesuai dengan nama pokemon.
    Setiap game index disimpan dalam berkas teks dengan nama yang sesuai.
    */
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
    /*
    Bagian ini memproses dan menyimpan data item yang dipegang oleh pokemon ke dalam direktori yang bernama sesuai dengan nama pokemon.
    Setiap item disimpan dalam berkas teks dengan nama yang sesuai.
    */
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
    /*
    Bagian ini memproses dan menyimpan data gerakan (moves) pokemon ke dalam direktori yang bernama sesuai dengan nama pokemon.
    Setiap gerakan disimpan dalam berkas teks dengan nama yang sesuai.
    */
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
    /*
    Bagian ini seharusnya memproses dan menyimpan data sprite (gambar) pokemon, tetapi dalam kode ini hanya membuat berkas kosong
    dengan ekstensi .png tanpa mengisi dengan data sprite.
    */
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
    /*
    Bagian ini memproses dan menyimpan data statistik pokemon ke dalam direktori yang bernama sesuai dengan nama pokemon.
    Setiap statistik disimpan dalam berkas teks dengan nama yang sesuai.
    */ 
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
    /*
    Bagian ini memproses dan menyimpan data tipe pokemon ke dalam direktori yang bernama sesuai dengan nama pokemon.
    Setiap tipe disimpan dalam berkas teks dengan nama yang sesuai.
    */
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




/*
Fungsi jsonParser pada kode di atas bertanggung jawab untuk melakukan parsing (mengurai) teks JSON yang diterima
dari objek req_t dan kemudian memproses hasil parsing menggunakan fungsi process_value.

Penting untuk memastikan bahwa teks JSON yang diberikan kepada fungsi ini adalah dalam format yang benar.
Jika teks JSON tidak valid, maka parsing akan gagal dan mungkin menyebabkan kesalahan saat pemrosesan lebih lanjut.
Selain itu, struktur dan konten teks JSON harus sesuai dengan harapan fungsi process_value agar
hasil pemrosesan sesuai dengan yang diinginkan.
*/
void jsonParser(req_t req){
    json_value *parsed_json = json_parse(req.text, strlen(req.text));
    process_value(parsed_json, 0);
    json_value_free(parsed_json);
}


/*
Fungsi xmp_getattr pada kode di atas adalah implementasi dari operasi getattr yang digunakan dalam sistem berkas
FUSE (Filesystem in Userspace). Operasi getattr digunakan untuk mendapatkan atribut (metadata) dari sebuah file atau direktori.

Fungsi ini pada dasarnya mengambil path relatif dari file atau direktori yang akan diperiksa,
mengonstruksi path absolut menggunakan dirpath, dan kemudian menggunakan lstat untuk mendapatkan atributnya.
Jika ada kesalahan dalam pemanggilan lstat, maka fungsi mengembalikan kode kesalahan. Jika sukses, atribut disimpan dalam struktur stbuf.
*/
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



/*
Fungsi xmp_readdir adalah implementasi dari operasi readdir yang digunakan dalam sistem berkas FUSE (Filesystem in Userspace).
Operasi readdir digunakan untuk membaca isi dari sebuah direktori. 

Fungsi ini pada dasarnya membaca isi dari direktori yang diberikan, mengonstruksi path absolut menggunakan dirpath,
membuka direktori tersebut, membaca entri-entri di dalamnya, dan mengisi informasi entri ke dalam buffer. Setelah selesai,
direktori ditutup dan fungsi mengembalikan nilai 0.
*/
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



/*
Fungsi xmp_read adalah implementasi dari operasi read yang digunakan dalam sistem berkas FUSE (Filesystem in Userspace).
Operasi read digunakan untuk membaca isi dari sebuah file. 

Fungsi ini pada dasarnya membuka file yang diberikan, membaca data dari file menggunakan pread, dan menyimpan data ke dalam buffer buf.
Setelah selesai, file ditutup dan fungsi mengembalikan nilai yang mengindikasikan berhasil tidaknya operasi read.
*/
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



/*
Fungsi xmp_mkdir adalah implementasi dari operasi mkdir yang digunakan dalam sistem berkas FUSE (Filesystem in Userspace).
Operasi mkdir digunakan untuk membuat direktori baru. 

Fungsi ini bertanggung jawab untuk membuat direktori baru dengan nama dan mode izin yang telah ditentukan. 
Jika berhasil, ia mengembalikan 0; jika tidak, mengembalikan nilai negatif yang mengindikasikan jenis kesalahan.
*/
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



/*
Mendefinisikan sebuah struct fuse_operations yang akan digunakan untuk menghubungkan fungsi-fungsi operasi berkas dari
FUSE dengan implementasi kustom yang telah dibuat dalam program 
*/
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
