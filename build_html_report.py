"""
Генератор интерактивного научно-технического HTML-отчета
на основе REPORT_5BK_LWD_HORIZONTAL_GEOSTEERING.md

Организация: НПФ «АМК Горизонт»
Разработчик: Заместитель начальника ОП Смирнов С. Г.
Дата: Октябрь 2026 г.
"""

import os

html_template = """<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>НПФ «АМК Горизонт» | Научно-технический отчет: 5БК LWD в горизонтальных скважинах</title>
  
  <!-- MathJax для рендеринга формул -->
  <script src="https://polyfill.io/v3/polyfill.min.js?features=es6"></script>
  <script id="MathJax-script" async src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"></script>

  <style>
    :root {
      --primary: #1a365d;
      --primary-light: #2b6cb0;
      --accent: #319795;
      --accent-dark: #234e52;
      --warning: #c53030;
      --warning-bg: #fff5f5;
      --info-bg: #ebf8fa;
      --bg: #f7fafc;
      --card-bg: #ffffff;
      --text: #2d3748;
      --text-muted: #718096;
      --border: #e2e8f0;
      --shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
      --shadow-lg: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);
    }

    * { box-sizing: border-box; margin: 0; padding: 0; }

    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
      background-color: var(--bg);
      color: var(--text);
      line-height: 1.65;
      padding: 0;
    }

    /* Верхний колонтитул */
    header.header {
      background: linear-gradient(135deg, #0f2b48 0%, #1a365d 100%);
      color: #ffffff;
      padding: 40px 20px 30px;
      border-bottom: 4px solid var(--accent);
      box-shadow: var(--shadow);
    }
    .header-container {
      max-width: 1280px;
      margin: 0 auto;
    }
    .badge-org {
      display: inline-block;
      background: rgba(255, 255, 255, 0.15);
      border: 1px solid rgba(255, 255, 255, 0.3);
      padding: 4px 12px;
      border-radius: 4px;
      font-size: 0.85rem;
      letter-spacing: 0.05em;
      text-transform: uppercase;
      margin-bottom: 15px;
      font-weight: 600;
    }
    .header h1 {
      font-size: 1.85rem;
      font-weight: 700;
      line-height: 1.3;
      margin-bottom: 12px;
    }
    .header h2 {
      font-size: 1.2rem;
      font-weight: 400;
      color: #cbd5e0;
      margin-bottom: 20px;
    }
    .meta-bar {
      display: flex;
      flex-wrap: wrap;
      gap: 24px;
      font-size: 0.9rem;
      color: #a0aec0;
      border-top: 1px solid rgba(255, 255, 255, 0.15);
      padding-top: 15px;
    }
    .meta-bar strong { color: #edf2f7; }

    /* Основной макет */
    .layout-container {
      max-width: 1280px;
      margin: 30px auto;
      padding: 0 20px;
      display: grid;
      grid-template-columns: 280px 1fr;
      gap: 30px;
      align-items: start;
    }

    /* Навигация (Сайдбар) */
    aside.sidebar {
      position: sticky;
      top: 20px;
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 20px;
      box-shadow: var(--shadow);
      font-size: 0.88rem;
    }
    .sidebar h3 {
      font-size: 0.95rem;
      text-transform: uppercase;
      letter-spacing: 0.05em;
      color: var(--primary);
      margin-bottom: 12px;
      padding-bottom: 8px;
      border-bottom: 2px solid var(--border);
    }
    .sidebar ul {
      list-style: none;
    }
    .sidebar li {
      margin-bottom: 8px;
    }
    .sidebar a {
      color: var(--text);
      text-decoration: none;
      display: block;
      padding: 6px 10px;
      border-radius: 4px;
      transition: all 0.2s;
    }
    .sidebar a:hover {
      background: #edf2f7;
      color: var(--primary-light);
    }

    /* Контент */
    main.content {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 40px;
      box-shadow: var(--shadow);
    }

    section {
      margin-bottom: 45px;
    }
    section:last-child {
      margin-bottom: 0;
    }

    h2.section-title {
      font-size: 1.45rem;
      color: var(--primary);
      border-bottom: 2px solid var(--accent);
      padding-bottom: 8px;
      margin-bottom: 20px;
      display: flex;
      align-items: center;
      gap: 10px;
    }
    h3.subsection-title {
      font-size: 1.15rem;
      color: var(--primary-light);
      margin: 25px 0 12px;
    }

    p { margin-bottom: 14px; }
    ul, ol { margin-bottom: 16px; padding-left: 24px; }
    li { margin-bottom: 6px; }

    /* Таблицы */
    .table-wrapper {
      overflow-x: auto;
      margin: 20px 0;
      border: 1px solid var(--border);
      border-radius: 6px;
    }
    table {
      width: 100%;
      border-collapse: collapse;
      text-align: left;
      font-size: 0.9rem;
    }
    th {
      background: #1a365d;
      color: white;
      padding: 12px 14px;
      font-weight: 600;
    }
    td {
      padding: 10px 14px;
      border-bottom: 1px solid var(--border);
    }
    tr:nth-child(even) td {
      background-color: #f8fafc;
    }
    tr:hover td {
      background-color: #edf2f7;
    }

    /* Инфобоксы / Карточки */
    .card {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 20px;
      margin: 20px 0;
      box-shadow: var(--shadow);
    }
    .card-info {
      background: var(--info-bg);
      border-left: 4px solid var(--accent);
    }
    .card-warning {
      background: var(--warning-bg);
      border-left: 4px solid var(--warning);
    }
    .card-primary {
      background: #f0f4f8;
      border-left: 4px solid var(--primary);
    }

    /* Изображения */
    figure.figure-box {
      margin: 25px 0;
      text-align: center;
      background: #fafafa;
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 15px;
    }
    figure.figure-box img {
      max-width: 100%;
      height: auto;
      border-radius: 4px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.08);
      cursor: zoom-in;
      transition: transform 0.2s;
    }
    figure.figure-box img:hover {
      transform: scale(1.005);
    }
    figcaption {
      font-size: 0.88rem;
      color: var(--text-muted);
      margin-top: 10px;
      font-style: italic;
    }

    /* Метрики и акценты */
    .metric-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 15px;
      margin: 20px 0;
    }
    .metric-card {
      background: #ffffff;
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 15px;
      text-align: center;
      border-top: 3px solid var(--accent);
      box-shadow: var(--shadow);
    }
    .metric-val {
      font-size: 1.5rem;
      font-weight: 700;
      color: var(--primary);
      margin: 5px 0;
    }
    .metric-label {
      font-size: 0.82rem;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    /* Модальное окно просмотра картинок (Lightbox) */
    .modal {
      display: none;
      position: fixed;
      z-index: 1000;
      left: 0;
      top: 0;
      width: 100%;
      height: 100%;
      background-color: rgba(0, 0, 0, 0.85);
      justify-content: center;
      align-items: center;
      cursor: zoom-out;
    }
    .modal-content {
      max-width: 95%;
      max-height: 95%;
      border-radius: 6px;
      box-shadow: 0 4px 20px rgba(0,0,0,0.5);
    }

    /* Печать */
    @media print {
      body { background: white; color: black; }
      header.header { background: none; color: black; border-bottom: 2px solid black; padding: 10px 0; }
      .header h1 { font-size: 1.4rem; color: black; }
      .header h2 { color: #333; }
      .meta-bar { color: #333; border-top: 1px solid #ccc; }
      .badge-org { border: 1px solid black; color: black; }
      .layout-container { display: block; padding: 0; margin: 0; }
      aside.sidebar { display: none; }
      main.content { border: none; box-shadow: none; padding: 0; }
      figure.figure-box img { max-width: 100% !important; }
      .card { border: 1px solid #ccc; box-shadow: none; }
    }

    @media (max-width: 900px) {
      .layout-container { grid-template-columns: 1fr; }
      aside.sidebar { position: static; margin-bottom: 25px; }
    }
  </style>
</head>
<body>

  <!-- ВЕРХНИЙ КОЛОНТИТУЛ -->
  <header class="header">
    <div class="header-container">
      <div class="badge-org">НПФ «АМК Горизонт»</div>
      <h1>Научно-технический отчет: Применение модуля 5БК LWD (Ø 120 мм, L = 5.0 м) для геонавигации и геофизических исследований в горизонтальных скважинах</h1>
      <h2>Физическое обоснование радиального зондирования, моделирование влияния границ пласта и оценка ограничений проактивной геонавигации при отстоянии от долота</h2>
      <div class="meta-bar">
        <div><strong>Организация:</strong> НПФ «АМК Горизонт»</div>
        <div><strong>Разработчик:</strong> Заместитель начальника ОП Смирнов С. Г.</div>
        <div><strong>Дата:</strong> Октябрь 2026 г.</div>
        <div><strong>Исполнение:</strong> Неазимутальное (кольцевое)</div>
      </div>
    </div>
  </header>

  <!-- ОСНОВНОЙ КОНТЕЙНЕР -->
  <div class="layout-container">
    
    <!-- Сайдбар-навигация -->
    <aside class="sidebar">
      <h3>Содержание отчета</h3>
      <ul>
        <li><a href="#sec1">1. Введение и ниша метода</a></li>
        <li><a href="#sec2">2. Радиальное зондирование на кольцах</a></li>
        <li><a href="#sec3">3. Чертеж компоновки 5.0 м</a></li>
        <li><a href="#sec4">4. Горизонтальная скважина и пласт</a></li>
        <li><a href="#sec5">5. Сравнение: ЭМК LWD vs 5БК LWD</a></li>
        <li><a href="#sec6">6. Кинематика плеча ВЗД 15–16 м</a></li>
        <li><a href="#sec7">7. Задел для 2.5D/3D инверсии</a></li>
        <li><a href="#sec8">8. Итоговое заключение</a></li>
      </ul>

      <div style="margin-top: 25px; padding-top: 15px; border-top: 1px solid var(--border); font-size: 0.8rem; color: var(--text-muted);">
        <strong>Статус модуля:</strong><br>
        Параметрически оптимизирован.<br>
        Длина: <strong>5.00 м</strong> (L = 5000 мм)<br>
        Диаметр: <strong>120 мм</strong> (Ø 120)<br>
        Глубинность 5БК-5: <strong>R₅₀ = 0.86 м</strong>, <strong>R₉₀ = 2.03 м</strong>
      </div>
    </aside>

    <!-- Основной контент -->
    <main class="content">

      <!-- РАЗДЕЛ 1 -->
      <section id="sec1">
        <h2 class="section-title">1. Введение и актуальность задачи</h2>
        <p>В производственном парке <strong>НПФ «АМК Горизонт»</strong> создан и серийно эксплуатируется высокотехнологичный прибор <strong>электромагнитного каротажа в процессе бурения (ЭМК LWD)</strong>. Комплекс надежно решает задачи геонавигации и количественной оценки удельного электрического сопротивления (УЭС) пород в песчано-глинистых разрезах в диапазоне УЭС до <strong>2000 Ом·м</strong>.</p>
        
        <div class="card card-warning">
          <strong>Проблема высокоомных коллекторов:</strong>
          В карбонатных отложениях (известняки, доломиты, рифовые массивы Восточной Сибири, Волго-Урала, Тимано-Печоры), а также в разрезах с плотными ангидритами и солями, истинное УЭС пород регулярно превышает <strong>2000 … 50 000 Ом·м</strong>. В таких интервалах метод ЭМК LWD испытывает физическое насыщение: сигнал вихревых токов пропорционален электропроводности среды (\(\\sigma = 1/\\rho\)), падая ниже <strong>0.5 … 0.1 мСм/м</strong>, и полезный сигнал тонет в аппаратных шумах и влиянии диэлектрической проницаемости (\\(\\varepsilon\\)). Прибор «слепнет», выходя на горизонтальную полку.
        </div>

        <p><strong>Цель проекта:</strong> Разработать цифровой модуль бокового каротажа <strong>5БК LWD (Ø 120 мм, L = 5.0 м)</strong> в неазимутальном исполнении как прямой инструмент измерения истинного УЭС и геофизического сопровождения в высокоомных карбонатах, где электромагнитный каротаж бессилен.</p>
      </section>

      <!-- РАЗДЕЛ 2 -->
      <section id="sec2">
        <h2 class="section-title">2. Физический принцип радиального зондирования в неазимутальном исполнении</h2>
        <p>Базовый вариант прибора 5БК представляет собой <strong>неазимутальный прибор с кольцевой конфигурацией электродов</strong> на стеклопластиковом корпусе диаметром <strong>120 мм</strong> (всего 19 электродов, минимальный изоляционный зазор между ними <strong>116.3 мм</strong>). Применение секторальных электродов рассматривается отдельно для перспективных специализированных азимутальных модификаций.</p>

        <p>Принципиально важно, что для получения 5 разноглубинных кривых прибор использует <strong>последовательное программное переключение диполей возбуждения</strong>:</p>

        <div class="metric-grid">
          <div class="metric-card">
            <div class="metric-label">5БК-1 (Сверхмелкий)</div>
            <div class="metric-val">R₅₀ = 0.29 м</div>
            <div style="font-size:0.8rem; color:#718096;">Диполь A₁ → A₂ (прискважинная зона)</div>
          </div>
          <div class="metric-card">
            <div class="metric-label">5БК-2 (Мелкий)</div>
            <div class="metric-val">R₅₀ = 0.36 м</div>
            <div style="font-size:0.8rem; color:#718096;">Диполь A₂ → A₃</div>
          </div>
          <div class="metric-card">
            <div class="metric-label">5БК-3 (Средний)</div>
            <div class="metric-val">R₅₀ = 0.47 м</div>
            <div style="font-size:0.8rem; color:#718096;">Диполь A₃ → A₄</div>
          </div>
          <div class="metric-card">
            <div class="metric-label">5БК-4 (Глубокий)</div>
            <div class="metric-val">R₅₀ = 0.60 м</div>
            <div style="font-size:0.8rem; color:#718096;">Диполь A₄ → A₅</div>
          </div>
          <div class="metric-card" style="border-top-color: #c53030;">
            <div class="metric-label">5БК-5 (Сверхглубокий)</div>
            <div class="metric-val">R₅₀ = 0.86 м</div>
            <div style="font-size:0.8rem; color:#c53030; font-weight:600;">Выход на пласт R₉₀ = 2.03 м</div>
          </div>
        </div>

        <div class="card card-primary">
          <strong>Математическая фокусировка в цифровом процессоре:</strong>
          Условие идеальной фокусировки тока в горизонтальный пластовый диск — полное зануление продольного градиента в скважине: \\(U_{MN} \\equiv 0\\). В процессоре вычисляется:
          $$K_{focus, i} = \\frac{U_{MN}^{(0)}}{U_{MN}^{(i)}}, \\qquad U_{focus, i} = U_{NN_y}^{(0)} - K_{focus, i} \\cdot U_{NN_y}^{(i)}, \\qquad \\rho_{ki} = K_{зi} \\cdot \\frac{U_{focus, i}}{I_0}$$
          Это обеспечивает идеальную фокусировку вплоть до контрастов сопротивлений <strong>200 000 : 1</strong>.
        </div>
      </section>

      <!-- РАЗДЕЛ 3: ЧЕРТЕЖ -->
      <section id="sec3">
        <h2 class="section-title">3. Сборочный чертеж и спецификация геометрии (L = 5.00 м, Ø 120 мм)</h2>
        <p>По результатам численного сканирования 17 вариантов длины (от 3.0 до 7.0 м) точка оптимального инженерного компромисса соответствует общей длине <strong>L = 5.00 м</strong> (полуразмах ±2500 мм от центра A₀).</p>

        <figure class="figure-box">
          <img src="sonde_5bk_120mm_5m_blueprint.png" alt="Сборочный чертеж решетки 5БК 120 мм 5.0 м" onclick="openModal(this)">
          <figcaption>Рис. 1. Сборочный чертеж электродной решетки 5БК (Ø 120 мм, L = 5.00 м). Нажмите на изображение для полноэкранного просмотра.</figcaption>
        </figure>

        <div class="table-wrapper">
          <table>
            <thead>
              <tr>
                <th>Электрод</th>
                <th>Функциональное назначение</th>
                <th>Расстояние от центра A₀</th>
                <th>Длина кольца</th>
                <th>Зазор до след. электрода</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><strong>A₀</strong></td>
                <td>Центральный токовый питающий</td>
                <td><strong>0.0 мм</strong></td>
                <td>50.0 мм</td>
                <td>145.0 мм (до M)</td>
              </tr>
              <tr>
                <td><strong>M (в/н)</strong></td>
                <td>Внутренний мониторный (градиент)</td>
                <td><strong>190.0 мм (0.19 м)</strong></td>
                <td>40.0 мм</td>
                <td>150.0 мм (до N)</td>
              </tr>
              <tr style="background:#fff5f5;">
                <td><strong>N (в/н)</strong></td>
                <td>Внешний мониторный (градиент)</td>
                <td><strong>380.0 мм (0.38 м)</strong></td>
                <td>40.0 мм</td>
                <td><strong style="color:#c53030;">116.3 мм (до A₁) [МИН. ЗАЗОР]</strong></td>
              </tr>
              <tr>
                <td><strong>A₁ (в/н)</strong></td>
                <td>Экран зонда 1 (малый зонд)</td>
                <td><strong>541.3 мм (0.54 м)</strong></td>
                <td>50.0 мм</td>
                <td>133.7 мм (до A₂)</td>
              </tr>
              <tr>
                <td><strong>A₂ (в/н)</strong></td>
                <td>Экран зонда 2</td>
                <td><strong>725.0 мм (0.73 м)</strong></td>
                <td>50.0 мм</td>
                <td>168.6 мм (до A₃)</td>
              </tr>
              <tr>
                <td><strong>A₃ (в/н)</strong></td>
                <td>Экран зонда 3 (средний зонд)</td>
                <td><strong>951.1 мм (0.95 м)</strong></td>
                <td>65.0 мм</td>
                <td>210.2 мм (до A₄)</td>
              </tr>
              <tr>
                <td><strong>A₄ (в/н)</strong></td>
                <td>Экран зонда 4</td>
                <td><strong>1233.8 мм (1.23 м)</strong></td>
                <td>80.0 мм</td>
                <td>263.3 мм (до A₅)</td>
              </tr>
              <tr>
                <td><strong>A₅ (в/н)</strong></td>
                <td>Экран зонда 5 (глубокий зонд)</td>
                <td><strong>1587.1 мм (1.59 м)</strong></td>
                <td>100.0 мм</td>
                <td>277.2 мм (до A₆)</td>
              </tr>
              <tr>
                <td><strong>A₆ (в/н)</strong></td>
                <td>Концевой токовый заземлитель</td>
                <td><strong>2039.3 мм (2.04 м)</strong></td>
                <td>250.0 мм</td>
                <td>205.7 мм (до Ny)</td>
              </tr>
              <tr>
                <td><strong>Ny (в/н)</strong></td>
                <td>Удаленный опорный потенциальный</td>
                <td><strong>2435.0 мм (2.44 м)</strong></td>
                <td>130.0 мм</td>
                <td>0.0 мм (торец 2.50 м)</td>
              </tr>
            </tbody>
          </table>
        </div>
      </section>

      <!-- РАЗДЕЛ 4: ГОРИЗОНТАЛЬНАЯ СКВАЖИНА -->
      <section id="sec4">
        <h2 class="section-title">4. Поведение прибора в горизонтальной скважине при наличии соседнего пласта</h2>
        
        <div class="card card-info">
          <strong>Особенность условий LWD (в процессе бурения):</strong>
          В процессе бурения горизонтального ствола фильтрационная зона проникновения еще не успела сформироваться (время контакта раствора с породой — минуты/десятки минут). <strong>Влияние именно прискважинной зоны минимально</strong>, поэтому регистрируемое расхождение 5 кривых отражает в первую очередь приближение к границам вмещающих пластов (Shoulder Bed Effect).
        </div>

        <p>В горизонтальной скважине сфокусированный токовый диск рассекает пласт вертикально на 360° вкрест напластования:</p>
        <ul>
          <li><strong>Приближение к проводящей глинистой кровле (\\(\\rho_{кол} = 50\\), \\(\\rho_{гл} = 2\\) Ом·м):</strong> Формируется классический «понижающий веер» геонавигации:
            $$\\rho_{k1} > \\rho_{k2} > \\rho_{k3} > \\rho_{k4} > \\rho_{k5}$$
            Глубокий зонд 5БК-5 фиксирует кровлю первым на удалении <strong>1.5 … 2.0 м</strong>. Степень расхождения веера прямо пропорциональна расстоянию до границы (DTB — Distance to Bed).
          </li>
          <li><strong>Приближение к высокоомному экрану/ангидриту:</strong> Ток отталкивается, глубокие зонды экранируются, формируя обратный веер: \\(\\rho_{k5} > \\rho_{k4} > \\dots > \\rho_{k1}\\).</li>
          <li><strong>Движение по центру мощного пласта (удаление &gt; 2.5 м):</strong> Все 5 кривых сливаются в единую линию, давая истинное омическое сопротивление пласта: \\(\\rho_{k1} \\approx \\dots \\approx \\rho_{k5} = \\rho_t\\).</li>
        </ul>

        <figure class="figure-box">
          <img src="parametric_scan_results_120mm.png" alt="Результаты параметрического сканирования 120 мм" onclick="openModal(this)">
          <figcaption>Рис. 2. Результаты параметрического сканирования: зависимость R₅₀ от длины, радиальная чувствительность g(r) и семейство 5 зондов при L = 5.0 м.</figcaption>
        </figure>
      </section>

      <!-- РАЗДЕЛ 5: СРАВНЕНИЕ ЭМК И 5БК -->
      <section id="sec5">
        <h2 class="section-title">5. Сравнительный анализ: ЭМК LWD vs 5БК LWD</h2>
        
        <div class="table-wrapper">
          <table>
            <thead>
              <tr>
                <th>Параметр сравнения</th>
                <th>Электромагнитный каротаж (ЭМК LWD)</th>
                <th>Боковой каротаж с мат. фокус. (5БК LWD)</th>
                <th>Вывод для НПФ «АМК Горизонт»</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><strong>Физический принцип</strong></td>
                <td>Вихревые токи: \\(S \\sim \\sigma = 1/\\rho\\)</td>
                <td>Падение потенциала: \\(U \\sim \\rho\\)</td>
                <td>Методы взаимно дополняют друг друга</td>
              </tr>
              <tr>
                <td><strong>Низкоомные разрезы (0.2 … 200 Ом·м)</strong></td>
                <td><strong style="color:#27ae60;">ИДЕАЛЬНО</strong> (высокий ток проводимости)</td>
                <td>Удовлетворительно (влияние скважины)</td>
                <td>В терригенных коллекторах ЭМК ведущий</td>
              </tr>
              <tr>
                <td><strong>Высокоомные карбонаты (2000 … 50 000+ Ом·м)</strong></td>
                <td><strong style="color:#c53030;">НАСЫЩЕНИЕ (СЛЕПНЕТ)</strong></td>
                <td><strong style="color:#27ae60;">ОСНОВНОЙ РАБОЧИЙ МЕТОД</strong></td>
                <td><strong>5БК закрывает «слепую зону» ЭМК на 100%</strong></td>
              </tr>
              <tr>
                <td><strong>Высокоминерализованные растворы</strong></td>
                <td>Фазовый сигнал искажается раствором</td>
                <td><strong style="color:#27ae60;">Работает отлично</strong> (соленый раствор улучшает контакт)</td>
                <td>В подсолевых залежах 5БК незаменим</td>
              </tr>
              <tr>
                <td><strong>Радиальная глубинность (DOI)</strong></td>
                <td>Около 1.5 м (не более 1.5 … 1.8 м)</td>
                <td>Около 1.5 м (R₅₀ = 0.86 м, R₉₀ = 2.03 м)</td>
                <td><strong>Одинаковый масштаб глубинности</strong></td>
              </tr>
              <tr>
                <td><strong>Диэлектрическая проницаемость (\\(\\varepsilon\\))</strong></td>
                <td>Сильная погрешность при \\(\\rho > 1000\\)</td>
                <td><strong style="color:#27ae60;">Полностью отсутствует</strong> (квазистатика)</td>
                <td>Истинное омическое УЭС без фазовых аномалий</td>
              </tr>
              <tr>
                <td><strong>Растворы на углеводородной основе (РУО)</strong></td>
                <td><strong style="color:#27ae60;">Работает отлично</strong> (поле проникает через нефть)</td>
                <td>Не работает (нужен проводящий контакт)</td>
                <td>На диэлектрических РУО метод ЭМК незаменим</td>
              </tr>
              <tr>
                <td><strong>Азимутальная селективность</strong></td>
                <td>Неазимутальный прибор</td>
                <td>Неазимутальный прибор (усреднение по 360°)</td>
                <td>Направление определяется по инклинометрии</td>
              </tr>
            </tbody>
          </table>
        </div>
      </section>

      <!-- РАЗДЕЛ 6: КИНЕМАТИКА И ОГРАНИЧЕНИЯ -->
      <section id="sec6">
        <h2 class="section-title">6. Кинематические ограничения проактивной геонавигации при отстоянии датчика 15–16 м за ВЗД</h2>
        
        <div class="card card-warning">
          <h3 style="color:#c53030; margin-bottom:10px;">Критический инженерный фактор компоновки КНБК:</h3>
          <p>В реальной компоновке низа бурильной колонны из-за габаритов забойного двигателя (ВЗД) центр модуля 5БК (как и модуль ЭМК LWD) вынужденно размещается на удалении <strong>15 … 16 метров от долота</strong>.</p>
          
          <p>Радиальная глубинность обоих приборов (как ЭМК LWD, так и 5БК LWD) составляет <strong>не более 1.5 метров</strong>. При угле сближения с кровлей или подошвой всего \\(\\alpha = 3^\\circ \\dots 4^\\circ\\) долото опережает измерительный центр датчика по вертикали (TVD) на:</p>
          
          <div style="text-align:center; font-size:1.15rem; font-weight:700; color:#c53030; margin:12px 0;">
            $$\\Delta h = L_{отст} \\cdot \\sin(\\alpha) = 15\\text{ м} \\cdot \\sin(3.5^\\circ) \\approx \\mathbf{0.9 \\dots 1.1\\text{ метра!}}$$
          </div>

          <p><strong>Физическое следствие:</strong> Когда глубокий зонд (с глубинностью 1.5 м) только начинает чувствовать границу пласта, долото уже подошло к ней практически вплотную (зазор менее 0.4 … 0.5 м) либо уже выскочило из пласта в глину на 5–10 метров вперед!</p>
          
          <p style="margin-bottom:0;"><strong>Инженерный вывод:</strong> При стандартной компоновке за длинным забойным двигателем <strong>полноценная упреждающая (проактивная) геонавигация в тонких пластах крайне затруднительна как для ЭМК, так и для 5БК</strong>.</p>
        </div>

        <h3 class="subsection-title">Истинное эксплуатационное назначение 5БК LWD в компоновке с ВЗД:</h3>
        <ol>
          <li><strong>Главная задача:</strong> Высокоточная петрофизическая оценка истинного удельного сопротивления (\\(R_t\\)) и характера насыщения (\\(S_w\\)) высокоомных карбонатов непосредственно в процессе бурения в реальном времени, где ЭМК «ослеп»;</li>
          <li><strong>Проводка в пластах достаточной мощности (&gt; 5 … 8 м):</strong> Контроль нахождения ствола внутри продуктивной пачки и своевременная реактивная фиксация приближения к границам;</li>
          <li><strong>Посадка в пласт (Landing):</strong> Контроль и подтверждение факта вскрытия кровли коллектора под малыми углами срезки (&lt; 1°);</li>
          <li><strong>Перспектива проактивной геонавигации:</strong> Переход к упреждающей проводке возможен при использовании коротких компоновок (с роторно-управляемыми системами — РУС), где отстояние датчика от долота сокращается до <strong>3 … 4 метров</strong>. При таком плече глубинность 1.5 м полностью обеспечивает проактивный маневр.</li>
        </ol>
      </section>

      <!-- РАЗДЕЛ 7: ЗАДЕЛ -->
      <section id="sec7">
        <h2 class="section-title">7. Научно-технический задел для количественной геонавигации</h2>
        <p>Для перехода от качественного наблюдения веера кривых к количественной геонавигации на буровой необходимо реализовать три научно-инженерных шага:</p>
        
        <div class="card card-primary">
          <ol>
            <li><strong>Разработка 2.5D / 3D моделирующего модуля:</strong> Текущий быстрый движок <code>BK_Engine.dll</code> решает осесимметричную задачу (вертикальная скважина). Для плоских границ горизонтального ствола необходим быстрый 2.5D FEM-решатель, рассчитывающий отклик 5БК при произвольном расстоянии \\(h\\) до кровли/подошвы.</li>
            <li><strong>Формирование базы предрасчитанных палеток DTB (Look-up Table):</strong> Предварительный расчет многомерной матрицы \\(\\rho_{ki} = f(DTB, \\rho_{кол}, \\rho_{вмещ}, \\alpha)\\) обеспечит мгновенную 1D-инверсию на полевом ноутбуке геонавигатора за доли секунды.</li>
            <li><strong>Протокол упаковки данных для MWD (гидроканал):</strong> По гидравлическому каналу связи невозможно передавать все 5 кривых. Забойный контроллер должен сворачивать данные и передавать на поверхность один синтетический параметр: <strong>«DTB = X.X м»</strong>.</li>
          </ol>
        </div>
      </section>

      <!-- РАЗДЕЛ 8: ВЫВОДЫ -->
      <section id="sec8">
        <h2 class="section-title">8. Итоговое заключение</h2>
        <div class="card card-info" style="font-size:0.95rem;">
          <ol>
            <li>Модуль <strong>5БК LWD (Ø 120 мм, L = 5.0 м)</strong> в неазимутальном исполнении признан оптимальным, физически и технологически обоснованным решением, закрывающим ключевую технологическую брешь электромагнитного каротажа в диапазоне УЭС <strong>от 2000 до 50 000+ Ом·м</strong>.</li>
            <li>При компоновке за длинным забойным двигателем (плечо 15–16 м от долота) приборы 5БК LWD и ЭМК LWD имеют сопоставимую глубинность (до 1.5 м) и решают в первую очередь задачу <strong>оперативной петрофизической оценки (\\(R_t, S_w\\)) в реальном времени бурения</strong>. Проактивная геонавигация в тонких пластах требует сокращения отстояния долота (РУС, 3–4 м).</li>
            <li>Комплексирование <strong>«ЭМК LWD (терригенный разрез) + 5БК LWD (карбонатный разрез)»</strong> обеспечит НПФ «АМК Горизонт» 100% охват любых типов геологических разрезов при горизонтальном бурении.</li>
          </ol>
        </div>
      </section>

    </main>
  </div>

  <!-- МОДАЛЬНОЕ ОКНО LIGHTBOX -->
  <div id="imageModal" class="modal" onclick="closeModal()">
    <img class="modal-content" id="modalImg">
  </div>

  <script>
    function openModal(element) {
      document.getElementById("modalImg").src = element.src;
      document.getElementById("imageModal").style.display = "flex";
    }
    function closeModal() {
      document.getElementById("imageModal").style.display = "none";
    }
  </script>

</body>
</html>
"""

output_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "REPORT_5BK_LWD_HORIZONTAL_GEOSTEERING.html")
with open(output_path, "w", encoding="utf-8") as f:
    f.write(html_template)

print(f"-> HTML-отчет успешно сформирован: {output_path}")
