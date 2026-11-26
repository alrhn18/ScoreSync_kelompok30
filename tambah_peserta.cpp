#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cctype>
#include <cstdlib>
using namespace std;

const int JUMLAH_JURI     = 3;
const int JUMLAH_KRITERIA = 3;

struct Juri {
    string idJuri;        
    string namaJuri;
};

struct Kriteria {
    string idKriteria;    
    string namaKriteria;
    double bobot;         
};

struct NodeSkor {         
    string idJuri;
    string idKriteria;
    int skorJuri;         
    NodeSkor* next;
};

struct NodePeserta {      
    string idPeserta;     
    string namaPeserta;
    double nilaiAkhir;    
    NodeSkor* child;
    NodePeserta* next;
};

struct NodeBST {          
    NodePeserta* peserta; 
    NodeBST* left;
    NodeBST* right;
};

string formatId(const string& prefix, int n) {
    string s = to_string(n);
    while (s.size() < 3) s = "0" + s;
    return prefix + "-" + s;
}

int bacaAngka(const string& pesan, int minV, int maxV) {
    int x = minV;
    bool valid = false;
    while (!valid) {
        cout << pesan;
        if (cin >> x) {
            cin.ignore(10000, '\n');
            if (x >= minV && x <= maxV) valid = true;
            else cout << "Input harus antara " << minV << " dan " << maxV << ".\n";
        } else if (cin.eof()) {
            cout << "\nInput berakhir.\n";
            x = minV;
            valid = true;
        } else {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Input harus berupa angka.\n";
        }
    }
    return x;
}

string bacaTeks(const string& pesan) {
    string hasil = "";
    bool valid = false;
    while (!valid) {
        string s;
        cout << pesan;
        if (!getline(cin, s)) {
            cout << "\nInput berakhir.\n";
            valid = true;
        } else {
            size_t a = s.find_first_not_of(" \t\r");
            if (a == string::npos) {
                cout << "Tidak boleh kosong.\n";
            } else {
                size_t b = s.find_last_not_of(" \t\r");
                hasil = s.substr(a, b - a + 1);
                valid = true;
            }
        }
    }
    return hasil;
}

void inisialisasiKriteria(Kriteria daftarKriteria[]) {
    const string nama[JUMLAH_KRITERIA]  = {"Kriteria 1", "Kriteria 2", "Kriteria 3"};
    const double bobot[JUMLAH_KRITERIA] = {0.4, 0.3, 0.3};
    for (int k = 0; k < JUMLAH_KRITERIA; k++) {
        daftarKriteria[k].idKriteria   = formatId("KRT", k + 1);
        daftarKriteria[k].namaKriteria = nama[k];
        daftarKriteria[k].bobot        = bobot[k];
    }
}

void inputJuri(Juri daftarJuri[], const Kriteria daftarKriteria[]) {
    cout << "Input Nama Juri:\n";
    for (int j = 0; j < JUMLAH_JURI; j++) {
        daftarJuri[j].idJuri   = formatId("JRI", j + 1);
        daftarJuri[j].namaJuri = bacaTeks("Nama juri ke-" + to_string(j + 1) + ": ");
    }
    cout << "\nKriteria penilaian:\n";
    for (int k = 0; k < JUMLAH_KRITERIA; k++) {
        cout << "  " << daftarKriteria[k].idKriteria << " " << daftarKriteria[k].namaKriteria
             << " (bobot " << (int)(daftarKriteria[k].bobot * 100 + 0.5) << "%)\n";
    }
}

NodeSkor* cariSkor(NodePeserta* p, const string& idJuri, const string& idKriteria) {
    NodeSkor* s = p->child;
    while (s != NULL && !(s->idJuri == idJuri && s->idKriteria == idKriteria)) {
        s = s->next;
    }
    return s;
}

void setSkor(NodePeserta* p, const string& idJuri, const string& idKriteria, int skor) {
    NodeSkor* s = cariSkor(p, idJuri, idKriteria);
    if (s != NULL) {
        s->skorJuri = skor;
    } else {
        NodeSkor* baru = new NodeSkor;
        baru->idJuri = idJuri;
        baru->idKriteria = idKriteria;
        baru->skorJuri = skor;
        baru->next = NULL;
        if (p->child == NULL) {
            p->child = baru;
        } else {
            NodeSkor* bantu = p->child;
            while (bantu->next != NULL) bantu = bantu->next;
            bantu->next = baru;
        }
    }
}

void hapusChild(NodePeserta* p) {
    while (p->child != NULL) {
        NodeSkor* hapus = p->child;
        p->child = p->child->next;
        delete hapus;
    }
}

double hitungNilaiAkhir(NodePeserta* p, const Kriteria daftarKriteria[]) {
    double jumlah[JUMLAH_KRITERIA] = {0, 0, 0};
    for (NodeSkor* s = p->child; s != NULL; s = s->next) {
        for (int k = 0; k < JUMLAH_KRITERIA; k++) {
            if (s->idKriteria == daftarKriteria[k].idKriteria) jumlah[k] += s->skorJuri;
        }
    }
    double na = 0;
    for (int k = 0; k < JUMLAH_KRITERIA; k++) {
        double rataKriteria = jumlah[k] / JUMLAH_JURI;
        na += rataKriteria * daftarKriteria[k].bobot;
    }
    return round(na * 100.0) / 100.0;   // dibulatkan 2 desimal
}

void tambahKeMLL(NodePeserta*& head, NodePeserta* p) {
    if (head == NULL) {
        head = p;
    } else {
        NodePeserta* bantu = head;
        while (bantu->next != NULL) bantu = bantu->next;
        bantu->next = p;
    }
}

bool lebihKecil(NodePeserta* a, NodePeserta* b) {
    bool hasil;
    if (a->nilaiAkhir != b->nilaiAkhir) hasil = a->nilaiAkhir < b->nilaiAkhir;
    else hasil = a->idPeserta < b->idPeserta;
    return hasil;
}

NodeBST* insertBST(NodeBST* node, NodePeserta* p) {
    if (node == NULL) {
        node = new NodeBST;
        node->peserta = p;
        node->left = NULL;
        node->right = NULL;
    } else if (lebihKecil(p, node->peserta)) {
        node->left = insertBST(node->left, p);
    } else {
        node->right = insertBST(node->right, p);
    }
    return node;
}

void hapusSemuaBST(NodeBST* n) {
    if (n != NULL) {
        hapusSemuaBST(n->left);
        hapusSemuaBST(n->right);
        delete n;
    }
}

void bebaskanMemori(NodePeserta*& head, NodeBST*& root) {
    hapusSemuaBST(root);
    root = NULL;
    while (head != NULL) {
        NodePeserta* p = head;
        head = head->next;
        hapusChild(p);
        delete p;
    }
}

void tambahPeserta(NodePeserta*& head, NodeBST*& root, int& counterPeserta,
                   const Juri daftarJuri[], const Kriteria daftarKriteria[]) {
    int n = bacaAngka("Jumlah peserta yang ingin ditambahkan (1-100): ", 1, 100);
    for (int i = 1; i <= n; i++) {
        cout << "\n--- Peserta ke-" << i << " dari " << n << " ---\n";
        string nama = bacaTeks("Nama peserta: ");

        NodePeserta* p = new NodePeserta;
        counterPeserta++;
        p->idPeserta = formatId("PSR", counterPeserta);
        p->namaPeserta = nama;
        p->nilaiAkhir = 0;
        p->child = NULL;
        p->next = NULL;

        cout << "ID peserta: " << p->idPeserta << "\n";
        for (int j = 0; j < JUMLAH_JURI; j++) {
            for (int k = 0; k < JUMLAH_KRITERIA; k++) {
                int skor = bacaAngka("Skor " + daftarJuri[j].namaJuri + " untuk " +
                                     daftarKriteria[k].namaKriteria + " (0-100): ", 0, 100);
                setSkor(p, daftarJuri[j].idJuri, daftarKriteria[k].idKriteria, skor);
            }
        }

        p->nilaiAkhir = hitungNilaiAkhir(p, daftarKriteria);   
        tambahKeMLL(head, p);                                  
        root = insertBST(root, p);                             

        cout << "Peserta " << p->namaPeserta << " ditambahkan. NA = "
             << fixed << setprecision(2) << p->nilaiAkhir << "\n";
    }
}

int main() {
    NodePeserta* head = NULL;
    NodeBST* root = NULL;
    int counterPeserta = 0;
    Juri daftarJuri[JUMLAH_JURI];
    Kriteria daftarKriteria[JUMLAH_KRITERIA];

    inisialisasiKriteria(daftarKriteria);
    inputJuri(daftarJuri, daftarKriteria);

    int pilihan;
    do {
        cout << "\nSCORESYNC: Tambah Peserta \n";
        cout << "1. Tambah Peserta\n";
        cout << "0. Keluar\n";
        pilihan = bacaAngka("Pilihan menu: ", -1000000, 1000000);
        cout << "\n";
        if (pilihan == 1) tambahPeserta(head, root, counterPeserta, daftarJuri, daftarKriteria);
        else if (pilihan != 0 && !cin.eof()) cout << "Pilihan Tidak Valid\n";
    } while (pilihan != 0 && !cin.eof());

    bebaskanMemori(head, root);
    cout << "Terima kasih. Program selesai.\n";
    return 0;
}