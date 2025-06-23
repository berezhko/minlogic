use std::collections::{HashMap, HashSet};

fn to_binary(n: usize, bits: usize) -> String {
    format!("{:0width$b}", n, width = bits)
}

fn count_ones(s: &str) -> usize {
    s.chars().filter(|&c| c == '1').count()
}

fn is_combinable(a: &str, b: &str) -> bool {
    a.chars().zip(b.chars()).filter(|(x, y)| x != y).count() == 1
}

fn combine(a: &str, b: &str) -> String {
    a.chars()
        .zip(b.chars())
        .map(|(x, y)| if x == y { x } else { '-' })
        .collect()
}

/// Проверяет, покрывает ли маска минтерм
fn mask_covers(mask: &str, minterm: &str) -> bool {
    mask.chars().zip(minterm.chars()).all(|(m, t)| m == '-' || m == t)
}

/// Генерирует все минтермы, покрываемые маской
fn mask_to_minterms(mask: &str) -> Vec<usize> {
    fn expand(mask: &str) -> Vec<String> {
        if let Some(pos) = mask.find('-') {
            let mut with_0 = mask.to_string();
            with_0.replace_range(pos..=pos, "0");
            let mut with_1 = mask.to_string();
            with_1.replace_range(pos..=pos, "1");

            let mut result = expand(&with_0);
            result.extend(expand(&with_1));
            result
        } else {
            vec![mask.to_string()]
        }
    }

    expand(mask)
        .into_iter()
        .map(|s| usize::from_str_radix(&s, 2).unwrap())
        .collect()
}

/// Этап 1: Нахождение прайм-импликантов
fn find_prime_implicants(minterms: &[usize], num_vars: usize) -> Vec<String> {
    let mut groups: HashMap<usize, Vec<String>> = HashMap::new();

    for &m in minterms {
        let b = to_binary(m, num_vars);
        groups.entry(count_ones(&b)).or_default().push(b);
    }

    let mut prime_implicants = HashSet::new();
    let mut marked = HashSet::new();
    let mut current = groups;

    loop {
        let mut next: HashMap<usize, Vec<String>> = HashMap::new();
        let mut combined = HashSet::new();

        let keys: Vec<_> = current.keys().copied().collect();
        for w in 0..keys.len() - 1 {
            if let (Some(g1), Some(g2)) = (current.get(&keys[w]), current.get(&keys[w + 1])) {
                for a in g1 {
                    for b in g2 {
                        if is_combinable(a, b) {
                            let c = combine(a, b);
                            next.entry(count_ones(&c)).or_default().push(c.clone());
                            marked.insert(a.clone());
                            marked.insert(b.clone());
                            combined.insert(c);
                        }
                    }
                }
            }
        }

        for group in current.values() {
            for term in group {
                if !marked.contains(term) {
                    prime_implicants.insert(term.clone());
                }
            }
        }

        if next.is_empty() {
            break;
        }

        current = HashMap::new();
        for term in combined {
            current.entry(count_ones(&term)).or_default().push(term);
        }
    }

    prime_implicants.into_iter().collect()
}

/// Этап 2–3: Построение таблицы покрытия и выбор существенных импликантов
fn minimize_function(minterms: &[usize], num_vars: usize) -> Vec<String> {
    let prime_implicants = find_prime_implicants(minterms, num_vars);

    // Таблица покрытия
    let mut table: HashMap<String, Vec<usize>> = HashMap::new();
    let mut minterm_to_pis: HashMap<usize, Vec<String>> = HashMap::new();

    for pi in &prime_implicants {
        let covered = mask_to_minterms(pi);
        let valid: Vec<_> = covered
            .into_iter()
            .filter(|m| minterms.contains(m))
            .collect();
        table.insert(pi.clone(), valid.clone());

        for m in valid {
            minterm_to_pis.entry(m).or_default().push(pi.clone());
        }
    }

    // Этап 3: Сущ. импликанты
    let mut essential: HashSet<String> = HashSet::new();
    let mut covered_minterms: HashSet<usize> = HashSet::new();

    for (&m, pis) in &minterm_to_pis {
        if pis.len() == 1 {
            let pi = &pis[0];
            essential.insert(pi.clone());
            if let Some(ms) = table.get(pi) {
                for &cm in ms {
                    covered_minterms.insert(cm);
                }
            }
        }
    }

    // Этап 4: Жадное покрытие остальных минтермов
    let mut uncovered: HashSet<_> = minterms
        .iter()
        .copied()
        .filter(|m| !covered_minterms.contains(m))
        .collect();
    let mut selected = essential.clone();

    while !uncovered.is_empty() {
        let best = prime_implicants
            .iter()
            .filter(|pi| !selected.contains(*pi))
            .max_by_key(|pi| {
                table
                    .get(*pi)
                    .unwrap()
                    .iter()
                    .filter(|m| uncovered.contains(m))
                    .count()
            });

        if let Some(best_pi) = best {
            selected.insert(best_pi.clone());
            if let Some(ms) = table.get(best_pi) {
                for &m in ms {
                    uncovered.remove(&m);
                }
            }
        } else {
            break;
        }
    }

    selected.into_iter().collect()
}

fn main() {
    // Пример: F(A,B,C,D) = Σm(0,1,2,5,6,7,8,9,10,14)
    let minterms = vec![0, 1, 2, 5, 6, 7, 8, 9, 10, 14];
    let num_vars = 4;

    let result = minimize_function(&minterms, num_vars);
    println!("Минимизированная функция в виде импликантов:");
    for term in result {
        println!("{}", term);
    }
}
