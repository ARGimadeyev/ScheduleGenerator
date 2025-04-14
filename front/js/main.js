document.addEventListener('DOMContentLoaded', () => {
    const hamburger = document.getElementById('hamburger');
    const sidebar = document.getElementById('sidebar');
    let isOpen = false;

    // Цвета для учителей
    const stringToColor = (str) => {
        let hash = 0;
        for (let i = 0; i < str.length; i++) {
            hash = str.charCodeAt(i) + ((hash << 8) - hash);
        }

        // Генерация оттенка с смещением
        const hue = Math.abs(hash % 360);

        // Фиксированные параметры для пастельных тонов
        const saturation = 60;
        const lightness = 85;

        // Автокоррекция для тёмных оттенков
        const safeHue = hue < 30 || hue > 330 ? (hue + 150) % 360 : hue;

        return `hsl(${safeHue}, ${saturation}%, ${lightness}%)`;
    };

    // Управление панелью
    hamburger.addEventListener('click', () => {
        isOpen = !isOpen;
        sidebar.style.right = isOpen ? '0' : '-320px';
    });

    // Загрузка данных
    fetch('../data/maps/class_colors.json')
        .then(response => response.json())
        .then(colors => {
            const grid = document.getElementById('classesGrid');
            Object.entries(colors).forEach(([className, color]) => {
                const btn = document.createElement('a');
                btn.className = 'class-button';
                btn.style.backgroundColor = color;
                btn.href = `pages/${className}.html`;
                btn.textContent = className;
                grid.appendChild(btn);
            });
        });

    fetch('../data/maps/teachers.json')
        .then(response => response.json())
        .then(teachers => {
            const list = document.getElementById('teachersList');
            Object.values(teachers).forEach(teacher => {
                const btn = document.createElement('a');
                btn.className = 'teacher-button';
                btn.href = `pages/teachers/${teacher}.html`;
                btn.textContent = teacher;
                btn.style.backgroundColor = stringToColor(teacher);
                list.appendChild(btn);
            });
        });

    // Обработчик скролла
    window.addEventListener('scroll', () => {
        document.body.classList.toggle('scrolled', window.scrollY > 50);
    });
});