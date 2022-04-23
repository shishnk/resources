use std::sync::mpsc;
use std::sync::mpsc::{Receiver, Sender};
use std::thread;
use std::thread::ThreadId;

// Структура для описания данных
struct Data {
    thread_id: ThreadId,
    msg: [char; 40],
}

fn main() {
    // Создаём два канала
    let (tx1, rx1): (Sender<Data>, Receiver<Data>) = mpsc::channel();
    let (tx2, rx2): (Sender<Data>, Receiver<Data>) = mpsc::channel();

    // Первый дочерний поток (т.н. P1)
    // -------------------------------------------------------------
    thread::spawn(move || {
        println!("Child 1 with {:?} is created.", thread::current().id());

        // Создаём данные для потока
        let mut data = Data {
            thread_id: thread::current().id(),
            msg: ['\0'; 40],
        };

        // Вот такое сообщение мы собираемся отправить
        let msg = "There is imposter among us.";

        // Конкатенируем строку. В Rust нету возможности
        // конкатенировать массивы символов, т.к. для этого
        // существуют отдельные сущности
        for (i, c) in msg.chars().take(40).enumerate() {
            data.msg[i] = c;
        }

        // Отправляем данные по первому каналу
        tx1.send(data)
            .expect("Can't send value with the first channel");

        println!(
            "Child 1 with {:?} have sent data with the first channel.",
            thread::current().id()
        );

        // Здесь удаляется tx1, потому что он находится во владении
        // этого потока, который заканчивается здесь
    });

    // Второй дочерний поток (т.н. P2)
    // -------------------------------------------------------------
    thread::spawn(move || {
        println!("Child 2 with pid {:?} is created.", thread::current().id());

        // Получаем данные из первого канала
        let mut data = rx1.recv().unwrap();

        println!(
            "Child 2 with {:?} have got data with the first channel from thread with {:?}.",
            thread::current().id(),
            data.thread_id
        );

        // Мы знаем, что это сообщение полностью не поместится в data.msg
        // Как раз не хватит места для имени убийцы :D
        let msg = " His name is DIO BRANDO.";

        // Получаем индекс первого нулевого символа
        let first_zero = data.msg.iter().position(|&c| c == '\0').unwrap_or(40);

        // Конкатенируем строки
        for (i, c) in msg.chars().take(40 - first_zero).enumerate() {
            data.msg[i + first_zero] = c;
        }

        tx2.send(data)
            .expect("Can't send value with the second channel");

        println!(
            "Child 2 with {:?} have sent data with the second channel.",
            thread::current().id()
        );

        // Здесь удаляется rx1 и tx2, потому что они находились во владении
        // этого потока, который заканчивается здесь
    });

    // Получаем данные из второго канала
    let data = rx2.recv().unwrap();

    println!(
        "Parent with {:?} with have got data with the first channel from thread with {:?}.",
        thread::current().id(),
        data.thread_id,
    );

    println!("Got message: {}", data.msg.iter().collect::<String>());

    // Здесь удаляется rx2, потому что он находился во владении
    // этого потока, который заканчивается здесь
}
