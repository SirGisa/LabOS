import java.util.*;

public class Lab7 {
    private static final Object lock = new Object();
    private static final Object sumMutex = new Object();
    private static final Object sumEvent = new Object();

    private static char[] array;
    private static List<Integer> digits = new ArrayList<>();
    private static List<Character> finalArray = new ArrayList<>();

    private static volatile boolean workDone = false;
    private static volatile boolean sumDone = false;
    private static int k;

    public static void main(String[] args) throws InterruptedException {
        Scanner scanner = new Scanner(System.in);

        // 1. Инициализация
        System.out.print("Введите размер массива: ");
        int size = scanner.nextInt();
        array = new char[size];

        // 2. Заполнение массива
        for (int i = 0; i < size; i++) {
                System.out.print("Введите символ " + (i + 1) + ": ");
                array[i] = scanner.next().charAt(0);
            }

        // 3. Вывод размера и массива
        System.out.println("Размер массива: " + array.length);
        System.out.println("Массив: " + Arrays.toString(array));

        // 4. Запуск потоков
        WorkThread work = new WorkThread();
        SumElementThread sumElement = new SumElementThread();

        // 6. Ввод k
        System.out.print("Введите число k < " + size + ": ");
        k = scanner.nextInt();

        work.start();
        sumElement.start();

        // 9. Приостановка main
        Thread.sleep(1500);

        // 10. Получение сигнала от work
        synchronized (lock) {
            while (!workDone) {
                lock.wait();  // ждем, пока work закончит
            }

            // 11. Критическая секция: вывод готового массива
            System.out.println("Итоговый массив:");
            for (char c : finalArray) {
                System.out.print(c + " ");
            }
            System.out.println();
        }

        // 12. Сигнал потоку SumElement через мьютекс
        synchronized (sumMutex) {
            sumMutex.notify();
        }

        // 14. Ждём событие (завершение потока SumElement)
        synchronized (sumEvent) {
            while (!sumDone) {
                sumEvent.wait();
            }
        }

        // 15. Вывод результата
        System.out.println("Среднее арифметическое " + k + " цифр: "
                + digits.stream().limit(k).mapToInt(Integer::intValue).average().orElse(0.0));

        scanner.close();
    }

    static class WorkThread extends Thread {
        public void run() {
            Scanner scanner = new Scanner(System.in);
            System.out.print("[Work] Введите задержку в миллисекундах: ");
            int delay = scanner.nextInt();

            for (char c : array) {
                if (Character.isDigit(c)) {
                    digits.add(Character.getNumericValue(c));
                    finalArray.add(c);
                }

                try {
                    Thread.sleep(delay);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            synchronized (lock) {
                workDone = true;
                lock.notify();
            }
            scanner.close();
        }
        
    }

    static class SumElementThread extends Thread {
        public void run() {
            // 1. Ждём мьютекс от main
            synchronized (sumMutex) {
                try {
                    sumMutex.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            // 2. Считаем среднее арифметическое
            double avg = digits.stream().limit(k).mapToInt(Integer::intValue).average().orElse(0.0);
            System.out.println("[SumElement] Среднее арифметическое: " + avg);

            // 3. Сигнал main'у
            synchronized (sumEvent) {
                sumDone = true;
                sumEvent.notify();
            }
        }
    }
}
