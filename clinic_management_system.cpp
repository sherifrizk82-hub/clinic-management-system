#include <bits/stdc++.h>
using namespace std;

/*
    Clinic Management System
    Based on the project requirements:
    - Person -> Patient / Doctor
    - Visit -> EmergencyVisit / NormalVisit
    - VisitHistory: linked list
    - WaitingRoom: priority queue built from scratch
    - Binary search by patient ID
    - Selection sort by name or age
    - Recursive total of visit fees
*/

int getInt(const string& message);
string getLine(const string& message);

class Visit {
protected:
    string date;
    string doctor;
    string diagnosis;
    double doctorFee;

public:
    Visit(string date, string doctor, string diagnosis, double doctorFee)
        : date(date), doctor(doctor), diagnosis(diagnosis), doctorFee(doctorFee) {}

    virtual ~Visit() {}

    virtual double calculateFee() const = 0;
    virtual Visit* clone() const = 0;

    void display() const {
        cout << date << " " << doctor << " " << diagnosis
             << " " << fixed << setprecision(0) << calculateFee() << " EGP\n";
    }
};

class EmergencyVisit : public Visit {
public:
    EmergencyVisit(string date, string doctor, string diagnosis, double doctorFee)
        : Visit(date, doctor, diagnosis, doctorFee) {}

    double calculateFee() const override {
        return doctorFee + 200;
    }

    Visit* clone() const override {
        return new EmergencyVisit(*this);
    }
};

class NormalVisit : public Visit {
public:
    NormalVisit(string date, string doctor, string diagnosis, double doctorFee)
        : Visit(date, doctor, diagnosis, doctorFee) {}

    double calculateFee() const override {
        return doctorFee;
    }

    Visit* clone() const override {
        return new NormalVisit(*this);
    }
};

class VisitHistory {
private:
    struct Node {
        Visit* visit;
        Node* next;

        Node(Visit* visit) : visit(visit), next(nullptr) {}
    };

    Node* head;

    double recursiveTotal(Node* current) const {
        if (current == nullptr)
            return 0;

        return current->visit->calculateFee()
             + recursiveTotal(current->next);
    }

    void clear() {
        Node* current = head;

        while (current != nullptr) {
            Node* next = current->next;
            delete current->visit;
            delete current;
            current = next;
        }

        head = nullptr;
    }

    void copyFrom(const VisitHistory& other) {
        Node* current = other.head;

        while (current != nullptr) {
            addVisit(current->visit->clone());
            current = current->next;
        }
    }

public:
    VisitHistory() : head(nullptr) {}

    VisitHistory(const VisitHistory& other) : head(nullptr) {
        copyFrom(other);
    }

    VisitHistory& operator=(const VisitHistory& other) {
        if (this != &other) {
            clear();
            copyFrom(other);
        }

        return *this;
    }

    ~VisitHistory() {
        clear();
    }

    void addVisit(Visit* visit) {
        Node* newNode = new Node(visit);

        if (head == nullptr) {
            head = newNode;
            return;
        }

        Node* current = head;
        while (current->next != nullptr)
            current = current->next;

        current->next = newNode;
    }

    void display() const {
        Node* current = head;

        while (current != nullptr) {
            current->visit->display();
            current = current->next;
        }
    }

    double totalPaid() const {
        return recursiveTotal(head);
    }

    int count() const {
        int result = 0;
        Node* current = head;

        while (current != nullptr) {
            result++;
            current = current->next;
        }

        return result;
    }
};

class Person {
protected:
    int id;
    string name;

public:
    Person(int id, string name) : id(id), name(name) {}
    virtual ~Person() {}

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    void setName(const string& newName) {
        name = newName;
    }

    virtual void displayInfo() const = 0;
};

class Patient : public Person {
private:
    int age;
    string phone;
    VisitHistory history;

public:
    Patient() : Person(0, ""), age(0), phone("") {}

    Patient(int id, string name, int age, string phone)
        : Person(id, name), age(age), phone(phone) {}

    int getAge() const {
        return age;
    }

    string getPhone() const {
        return phone;
    }

    int getVisitCount() const {
        return history.count();
    }

    void addVisit(Visit* visit) {
        history.addVisit(visit);
    }

    void displayInfo() const override {
        cout << "ID : " << id << '\n';
        cout << "Name : " << name << '\n';
        cout << "Age : " << age << '\n';
        cout << "Phone : " << phone << '\n';
        cout << "Visits : " << history.count() << '\n';
    }

    void displayHistory() const {
        cout << "VISIT HISTORY - " << name << " [linked list]\n";
        cout << "--------------------------------------------------\n";

        if (history.count() == 0) {
            cout << "No visits yet.\n";
        } else {
            history.display();
        }

        cout << "--------------------------------------------------\n";
        cout << "Total paid: " << fixed << setprecision(0)
             << history.totalPaid() << " EGP [recursive sum]\n";
    }

    bool operator<(const Patient& other) const {
        return name < other.name;
    }
};

class Doctor : public Person {
private:
    string specialization;
    double consultationFee;

public:
    Doctor() : Person(0, ""), specialization(""), consultationFee(0) {}

    Doctor(int id, string name, string specialization, double consultationFee)
        : Person(id, name),
          specialization(specialization),
          consultationFee(consultationFee) {}

    string getSpecialization() const {
        return specialization;
    }

    double getConsultationFee() const {
        return consultationFee;
    }

    void displayInfo() const override {
        cout << "ID : " << id << '\n';
        cout << "Name : " << name << '\n';
        cout << "Specialization : " << specialization << '\n';
        cout << "Consultation Fee : " << fixed << setprecision(0)
             << consultationFee << " EGP\n";
    }
};

class WaitingRoom {
private:
    struct WaitingPatient {
        int patientId;
        int priority;       // 2 = Emergency, 1 = Normal
        long long arrivalNo;

        WaitingPatient() : patientId(0), priority(0), arrivalNo(0) {}

        WaitingPatient(int patientId, int priority, long long arrivalNo)
            : patientId(patientId), priority(priority), arrivalNo(arrivalNo) {}
    };

    WaitingPatient* heap;
    int size;
    int capacity;
    long long nextArrival;

    bool higherPriority(const WaitingPatient& a,
                        const WaitingPatient& b) const {
        if (a.priority != b.priority)
            return a.priority > b.priority;

        return a.arrivalNo < b.arrivalNo;
    }

    void resize() {
        capacity *= 2;
        WaitingPatient* newHeap = new WaitingPatient[capacity];

        for (int i = 0; i < size; i++)
            newHeap[i] = heap[i];

        delete[] heap;
        heap = newHeap;
    }

    void swapItems(int a, int b) {
        WaitingPatient temp = heap[a];
        heap[a] = heap[b];
        heap[b] = temp;
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (!higherPriority(heap[index], heap[parent]))
                break;

            swapItems(index, parent);
            index = parent;
        }
    }

    void heapifyDown(int index) {
        while (true) {
            int left = index * 2 + 1;
            int right = index * 2 + 2;
            int best = index;

            if (left < size && higherPriority(heap[left], heap[best]))
                best = left;

            if (right < size && higherPriority(heap[right], heap[best]))
                best = right;

            if (best == index)
                break;

            swapItems(index, best);
            index = best;
        }
    }

public:
    WaitingRoom() : size(0), capacity(10), nextArrival(1) {
        heap = new WaitingPatient[capacity];
    }

    WaitingRoom(const WaitingRoom& other)
        : size(other.size),
          capacity(other.capacity),
          nextArrival(other.nextArrival) {
        heap = new WaitingPatient[capacity];

        for (int i = 0; i < size; i++)
            heap[i] = other.heap[i];
    }

    WaitingRoom& operator=(const WaitingRoom& other) {
        if (this != &other) {
            delete[] heap;

            size = other.size;
            capacity = other.capacity;
            nextArrival = other.nextArrival;

            heap = new WaitingPatient[capacity];

            for (int i = 0; i < size; i++)
                heap[i] = other.heap[i];
        }

        return *this;
    }

    ~WaitingRoom() {
        delete[] heap;
    }

    bool contains(int patientId) const {
        for (int i = 0; i < size; i++) {
            if (heap[i].patientId == patientId)
                return true;
        }

        return false;
    }

    void add(int patientId, int caseType) {
        if (size == capacity)
            resize();

        int priority = (caseType == 1 ? 2 : 1);

        heap[size] = WaitingPatient(patientId, priority, nextArrival++);
        heapifyUp(size);
        size++;
    }

    bool empty() const {
        return size == 0;
    }

    int sizeOf() const {
        return size;
    }

    int peekPatientId() const {
        if (size == 0)
            return -1;

        return heap[0].patientId;
    }

    int peekPriority() const {
        if (size == 0)
            return -1;

        return heap[0].priority;
    }

    int callNext() {
        if (size == 0)
            return -1;

        int patientId = heap[0].patientId;

        heap[0] = heap[size - 1];
        size--;

        if (size > 0)
            heapifyDown(0);

        return patientId;
    }

    int getPriorityForPatient(int patientId) const {
        for (int i = 0; i < size; i++) {
            if (heap[i].patientId == patientId)
                return heap[i].priority;
        }

        return -1;
    }
};

class Clinic {
private:
    Patient* patients;
    int patientCount;
    int patientCapacity;

    Doctor* doctors;
    int doctorCount;
    int doctorCapacity;

    WaitingRoom waitingRoom;

    void resizePatients() {
        patientCapacity *= 2;
        Patient* newPatients = new Patient[patientCapacity];

        for (int i = 0; i < patientCount; i++)
            newPatients[i] = patients[i];

        delete[] patients;
        patients = newPatients;
    }

    void resizeDoctors() {
        doctorCapacity *= 2;
        Doctor* newDoctors = new Doctor[doctorCapacity];

        for (int i = 0; i < doctorCount; i++)
            newDoctors[i] = doctors[i];

        delete[] doctors;
        doctors = newDoctors;
    }

    void sortPatientsById() {
        for (int i = 0; i < patientCount - 1; i++) {
            int minIndex = i;

            for (int j = i + 1; j < patientCount; j++) {
                if (patients[j].getId() < patients[minIndex].getId())
                    minIndex = j;
            }

            if (minIndex != i) {
                Patient temp = patients[i];
                patients[i] = patients[minIndex];
                patients[minIndex] = temp;
            }
        }
    }

    int binarySearchById(int id, int& comparisons) const {
        int left = 0;
        int right = patientCount - 1;
        comparisons = 0;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            comparisons++;

            if (patients[mid].getId() == id)
                return mid;

            if (patients[mid].getId() < id)
                left = mid + 1;
            else
                right = mid - 1;
        }

        return -1;
    }

    void printPatientArray(Patient* arr, int count, bool showPhone) const {
        cout << "ID NAME AGE";
        if (showPhone)
            cout << " PHONE";
        cout << '\n';

        cout << "- --------------- -";
        if (showPhone)
            cout << " -----------";
        cout << '\n';

        for (int i = 0; i < count; i++) {
            cout << arr[i].getId() << " "
                 << arr[i].getName() << " "
                 << arr[i].getAge();

            if (showPhone)
                cout << " " << arr[i].getPhone();

            cout << '\n';
        }

        cout << count << " patients.\n";
    }

public:
    Clinic()
        : patientCount(0), patientCapacity(10),
          doctorCount(0), doctorCapacity(10) {
        patients = new Patient[patientCapacity];
        doctors = new Doctor[doctorCapacity];
    }

    ~Clinic() {
        delete[] patients;
        delete[] doctors;
    }

    int getPatientIndex(int id) const {
        int comparisons = 0;
        return binarySearchById(id, comparisons);
    }

    bool addPatient(int id, string name, int age, string phone) {
        int comparisons = 0;

        if (binarySearchById(id, comparisons) != -1)
            return false;

        if (patientCount == patientCapacity)
            resizePatients();

        patients[patientCount] = Patient(id, name, age, phone);
        patientCount++;

        sortPatientsById();
        return true;
    }

    void viewAllPatients() const {
        if (patientCount == 0) {
            cout << "No patients.\n";
            return;
        }

        printPatientArray(patients, patientCount, true);
    }

    void searchPatient() const {
        int id = getInt("Patient ID: ");
        int comparisons = 0;

        int index = binarySearchById(id, comparisons);

        if (index == -1) {
            cout << "[X] No patient with ID " << id << ".\n";
            return;
        }

        cout << "Found in " << comparisons << " comparisons [binary search]\n";
        patients[index].displayInfo();
    }

    void deletePatient() {
        int id = getInt("Patient ID: ");
        int comparisons = 0;

        int index = binarySearchById(id, comparisons);

        if (index == -1) {
            cout << "[X] No patient with ID " << id << ".\n";
            return;
        }

        if (waitingRoom.contains(id)) {
            cout << "[X] Cannot delete a patient who is waiting.\n";
            return;
        }

        for (int i = index; i < patientCount - 1; i++)
            patients[i] = patients[i + 1];

        patientCount--;

        cout << "[OK] Patient " << id << " deleted.\n";
    }

    void addDoctor() {
        int id = getInt("ID : ");
        string name = getLine("Name : ");
        string specialization = getLine("Specialization : ");
        double fee = 0;

        while (true) {
            cout << "Consultation fee : ";
            if (cin >> fee && fee >= 0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "[X] Please enter a valid fee.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        for (int i = 0; i < doctorCount; i++) {
            if (doctors[i].getId() == id) {
                cout << "[X] Doctor ID already exists.\n";
                return;
            }
        }

        if (doctorCount == doctorCapacity)
            resizeDoctors();

        doctors[doctorCount] = Doctor(id, name, specialization, fee);
        doctorCount++;

        cout << "[OK] Doctor " << id << " added.\n";
    }

    void viewAllDoctors() const {
        if (doctorCount == 0) {
            cout << "No doctors.\n";
            return;
        }

        cout << "ID NAME SPECIALIZATION FEE\n";
        cout << "---------------------------\n";

        for (int i = 0; i < doctorCount; i++) {
            cout << doctors[i].getId() << " "
                 << doctors[i].getName() << " "
                 << doctors[i].getSpecialization() << " "
                 << fixed << setprecision(0)
                 << doctors[i].getConsultationFee() << " EGP\n";
        }
    }

    int getDoctorIndex(int id) const {
        for (int i = 0; i < doctorCount; i++) {
            if (doctors[i].getId() == id)
                return i;
        }

        return -1;
    }

    void viewPatientHistory() const {
        int id = getInt("Patient ID: ");
        int comparisons = 0;
        int index = binarySearchById(id, comparisons);

        if (index == -1) {
            cout << "[X] No patient with ID " << id << ".\n";
            return;
        }

        patients[index].displayHistory();
    }

    void sortPatients() const {
        if (patientCount == 0) {
            cout << "No patients.\n";
            return;
        }

        int choice = getInt("Sort by (1) name (2) age: ");

        if (choice != 1 && choice != 2) {
            cout << "[X] Invalid choice.\n";
            return;
        }

        Patient* arr = new Patient[patientCount];

        for (int i = 0; i < patientCount; i++)
            arr[i] = patients[i];

        long long comparisons = 0;

        // Selection sort written by hand.
        for (int i = 0; i < patientCount - 1; i++) {
            int minIndex = i;

            for (int j = i + 1; j < patientCount; j++) {
                comparisons++;

                bool smaller;

                if (choice == 1)
                    smaller = arr[j] < arr[minIndex];
                else
                    smaller = arr[j].getAge() < arr[minIndex].getAge();

                if (smaller)
                    minIndex = j;
            }

            if (minIndex != i) {
                Patient temp = arr[i];
                arr[i] = arr[minIndex];
                arr[minIndex] = temp;
            }
        }

        cout << "ID NAME AGE\n";
        cout << "----------------\n";

        for (int i = 0; i < patientCount; i++) {
            cout << arr[i].getId() << " "
                 << arr[i].getName() << " "
                 << arr[i].getAge() << '\n';
        }

        cout << "Sorted in " << comparisons
             << " comparisons [selection sort]\n";

        delete[] arr;
    }

    void addToWaitingRoom() {
        int id = getInt("Patient ID: ");
        int comparisons = 0;

        if (binarySearchById(id, comparisons) == -1) {
            cout << "[X] No patient with ID " << id << ".\n";
            return;
        }

        if (waitingRoom.contains(id)) {
            cout << "[X] Patient is already waiting.\n";
            return;
        }

        int type = getInt("Case type : (1) Emergency (2) Normal\nChoose : ");

        if (type != 1 && type != 2) {
            cout << "[X] Invalid case type.\n";
            return;
        }

        waitingRoom.add(id, type);

        int index = binarySearchById(id, comparisons);
        string typeName = (type == 1 ? "EMERGENCY" : "NORMAL");

        cout << "[OK] " << patients[index].getName()
             << " added as " << typeName << ".\n";
    }

    void viewWaitingRoom() const {
        if (waitingRoom.empty()) {
            cout << "[X] No patients waiting.\n";
            return;
        }

        // We make a temporary priority queue and repeatedly remove its top.
        // This prints the patients in true priority order without using std::priority_queue.
        WaitingRoom temp = waitingRoom;

        cout << "# PATIENT CASE ARRIVED\n";

        int position = 1;

        while (!temp.empty()) {
            int id = temp.callNext();

            int comparisons = 0;
            int index = binarySearchById(id, comparisons);

            int priority = waitingRoom.getPriorityForPatient(id);
            string typeName = (priority == 2 ? "EMERGENCY" : "NORMAL");

            cout << position++ << " "
                 << patients[index].getName() << " "
                 << typeName << '\n';
        }
    }

    void callNextPatient() {
        if (waitingRoom.empty()) {
            cout << "[X] No patients waiting.\n";
            return;
        }

        int patientId = waitingRoom.peekPatientId();
        int priority = waitingRoom.peekPriority();

        int comparisons = 0;
        int patientIndex = binarySearchById(patientId, comparisons);

        if (patientIndex == -1) {
            cout << "[X] Patient no longer exists.\n";
            return;
        }

        int type = (priority == 2 ? 1 : 2);
        string typeName = (type == 1 ? "EMERGENCY" : "NORMAL");

        cout << "> NOW CALLING: " << patients[patientIndex].getName()
             << " (" << typeName << ")\n";

        int doctorId = getInt("Doctor ID: ");
        int doctorIndex = getDoctorIndex(doctorId);

        if (doctorIndex == -1) {
            cout << "[X] No doctor with ID " << doctorId << ".\n";
            return;
        }

        string date = getLine("Date (YYYY-MM-DD): ");
        string diagnosis = getLine("Diagnosis: ");

        Visit* visit = nullptr;

        if (type == 1) {
            visit = new EmergencyVisit(
                date,
                doctors[doctorIndex].getName(),
                diagnosis,
                doctors[doctorIndex].getConsultationFee()
            );
        } else {
            visit = new NormalVisit(
                date,
                doctors[doctorIndex].getName(),
                diagnosis,
                doctors[doctorIndex].getConsultationFee()
            );
        }

        double fee = visit->calculateFee();

        // Only remove the patient after all visit information is valid.
        waitingRoom.callNext();

        patients[patientIndex].addVisit(visit);

        cout << "Doctor : " << doctors[doctorIndex].getName() << '\n';
        cout << "Diagnosis : " << diagnosis << '\n';
        cout << "Fee: " << fixed << setprecision(0)
             << fee << " EGP ["
             << (type == 1 ? "EmergencyVisit" : "NormalVisit")
             << "::calculateFee]\n";
        cout << "[OK] Visit added to history. "
             << waitingRoom.sizeOf() << " patient(s) still waiting.\n";
    }

    void patientsMenu() {
        int choice;

        do {
            cout << "\n------------- PATIENTS ------------------\n";
            cout << "1. Add patient\n";
            cout << "2. View all patients\n";
            cout << "3. Search by ID\n";
            cout << "4. Delete patient\n";
            cout << "5. View patient history\n";
            cout << "6. Sort patients\n";
            cout << "0. Back\n";
            cout << "-----------------------------------------\n";

            choice = getInt("Choose: ");

            if (choice == 1) {
                int id = getInt("ID : ");
                string name = getLine("Name : ");
                int age = getInt("Age : ");
                string phone = getLine("Phone : ");

                if (addPatient(id, name, age, phone))
                    cout << "[OK] Patient " << id << " added.\n";
                else
                    cout << "[X] Patient ID already exists.\n";
            } else if (choice == 2) {
                viewAllPatients();
            } else if (choice == 3) {
                searchPatient();
            } else if (choice == 4) {
                deletePatient();
            } else if (choice == 5) {
                viewPatientHistory();
            } else if (choice == 6) {
                sortPatients();
            } else if (choice != 0) {
                cout << "[X] Invalid choice.\n";
            }

        } while (choice != 0);
    }

    void doctorsMenu() {
        int choice;

        do {
            cout << "\n------------- DOCTORS -------------------\n";
            cout << "1. Add doctor\n";
            cout << "2. View all doctors\n";
            cout << "0. Back\n";
            cout << "-----------------------------------------\n";

            choice = getInt("Choose: ");

            if (choice == 1)
                addDoctor();
            else if (choice == 2)
                viewAllDoctors();
            else if (choice != 0)
                cout << "[X] Invalid choice.\n";

        } while (choice != 0);
    }

    void waitingRoomMenu() {
        int choice;

        do {
            cout << "\n---------- WAITING ROOM -----------------\n";
            cout << "1. Add to waiting room\n";
            cout << "2. View who is waiting\n";
            cout << "3. Call next patient\n";
            cout << "0. Back\n";
            cout << "-----------------------------------------\n";

            choice = getInt("Choose: ");

            if (choice == 1)
                addToWaitingRoom();
            else if (choice == 2)
                viewWaitingRoom();
            else if (choice == 3)
                callNextPatient();
            else if (choice != 0)
                cout << "[X] Invalid choice.\n";

        } while (choice != 0);
    }

    void run() {
        int choice;

        do {
            cout << "\n=========================================\n";
            cout << "       CLINIC MANAGEMENT SYSTEM\n";
            cout << "=========================================\n";
            cout << "1. Patients\n";
            cout << "2. Doctors\n";
            cout << "3. Waiting Room\n";
            cout << "0. Exit\n";
            cout << "-----------------------------------------\n";

            choice = getInt("Choose: ");

            if (choice == 1)
                patientsMenu();
            else if (choice == 2)
                doctorsMenu();
            else if (choice == 3)
                waitingRoomMenu();
            else if (choice != 0)
                cout << "[X] Invalid choice.\n";

        } while (choice != 0);
    }
};

int getInt(const string& message) {
    int value;

    while (true) {
        cout << message;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "[X] Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string getLine(const string& message) {
    string value;

    cout << message;
    getline(cin, value);

    while (value.empty()) {
        cout << "[X] Input cannot be empty.\n";
        cout << message;
        getline(cin, value);
    }

    return value;
}

int main() {
    Clinic clinic;
    clinic.run();

    return 0;
}
