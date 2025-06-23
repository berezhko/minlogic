use std::collections::{HashMap, HashSet, BTreeMap};

#[derive(Debug, Clone, PartialEq, Eq, Hash, PartialOrd, Ord)]
struct Implicant {
    mask: u32,    // 1 = значимый бит, 0 = don't care
    value: u32,   // Значение битов (только для значимых битов)
    bits: u8,     // Общее число битов
}

impl Implicant {
    fn new(minterm: u32, bits: u8) -> Self {
        Implicant {
            mask: (1 << bits) - 1,
            value: minterm,
            bits,
        }
    }

    fn count_ones(&self) -> u32 {
        (self.value & self.mask).count_ones()
    }

    fn covers(&self, minterm: u32) -> bool {
        (minterm & self.mask) == (self.value & self.mask)
    }

    fn combine(&self, other: &Self) -> Option<Self> {
        if self.bits != other.bits {
            return None;
        }

        let common_mask = self.mask & other.mask;
        let diff = (self.value ^ other.value) & common_mask;

        if diff.count_ones() == 1 {
            let new_mask = common_mask & !diff;
            Some(Implicant {
                mask: new_mask,
                value: self.value & new_mask,
                bits: self.bits,
            })
        } else {
            None
        }
    }

    fn to_string(&self) -> String {
        (0..self.bits).rev().map(|i| {
            let bit = 1 << i;
            if self.mask & bit == 0 {
                '-'
            } else if self.value & bit != 0 {
                '1'
            } else {
                '0'
            }
        }).collect()
    }
}

fn quine_mccluskey(minterms: &[u32], bits: u8) -> Vec<Implicant> {
    // Группировка минтермов по количеству единиц
    let mut groups: BTreeMap<u32, Vec<Implicant>> = BTreeMap::new();
    for &m in minterms {
        let imp = Implicant::new(m, bits);
        groups.entry(imp.count_ones()).or_default().push(imp);
    }

    let mut primes = Vec::new();
    let mut found = true;
    let mut current_groups = groups;

    while found {
        found = false;
        let mut next_groups: BTreeMap<u32, Vec<Implicant>> = BTreeMap::new();
        let mut used = HashSet::new();
        let keys: Vec<u32> = current_groups.keys().cloned().collect();

        for i in 0..keys.len() - 1 {
            let group1 = current_groups.get(&keys[i]).unwrap();
            let group2 = current_groups.get(&keys[i + 1]).unwrap();

            for imp1 in group1 {
                for imp2 in group2 {
                    if let Some(new_imp) = imp1.combine(imp2) {
                        let ones = new_imp.count_ones();
                        next_groups.entry(ones).or_default().push(new_imp.clone());
                        used.insert(imp1.clone());
                        used.insert(imp2.clone());
                        found = true;
                    }
                }
            }
        }

        // Добавляем неиспользованные импликанты в первичные
        for group in current_groups.values() {
            for imp in group {
                if !used.contains(imp) {
                    primes.push(imp.clone());
                }
            }
        }

        current_groups = next_groups;
    }

    // Добавляем оставшиеся импликанты из последней итерации
    for group in current_groups.values() {
        primes.extend(group.clone());
    }

    // Удаление дубликатов
    primes.sort_unstable();
    primes.dedup();
    primes
}

fn minimize(minterms: &[u32], bits: u8) -> Vec<Implicant> {
    let primes = quine_mccluskey(minterms, bits);
    if primes.is_empty() {
        return Vec::new();
    }

    // Построение таблицы покрытий
    let mut covers: Vec<Vec<bool>> = vec![vec![false; minterms.len()]; primes.len()];
    for (i, prime) in primes.iter().enumerate() {
        for (j, &m) in minterms.iter().enumerate() {
            covers[i][j] = prime.covers(m);
        }
    }

    // Поиск обязательных импликант
    let mut essential = Vec::new();
    let mut covered = vec![false; minterms.len()];

    for j in 0..minterms.len() {
        let covering: Vec<usize> = (0..primes.len())
            .filter(|&i| covers[i][j])
            .collect();

        if covering.len() == 1 {
            let i = covering[0];
            if !essential.contains(&i) {
                essential.push(i);
                for k in 0..minterms.len() {
                    if covers[i][k] {
                        covered[k] = true;
                    }
                }
            }
        }
    }

    // Жадный выбор оставшихся импликант
    while covered.iter().any(|&c| !c) {
        let mut best_i = None;
        let mut best_cover = 0;

        for i in 0..primes.len() {
            if essential.contains(&i) {
                continue;
            }

            let count = covers[i].iter()
                .enumerate()
                .filter(|&(j, &c)| c && !covered[j])
                .count();

            if count > best_cover {
                best_cover = count;
                best_i = Some(i);
            }
        }

        if let Some(i) = best_i {
            essential.push(i);
            for j in 0..minterms.len() {
                if covers[i][j] {
                    covered[j] = true;
                }
            }
        } else {
            break;
        }
    }

    essential.into_iter().map(|i| primes[i].clone()).collect()
}

fn main() {
    // Пример: f(a,b,c) = Σ(0, 1, 5, 7)
    let minterms = vec![0, 1, 5, 7];
    let bits = 3;
    let result = minimize(&minterms, bits);

    println!("Minimized function:");
    for imp in &result {
        println!("{}", imp.to_string());
    }
    println!();

    // Пример 2: Четвертичный сумматор (пример из Википедии)
    let minterms = vec![0, 1, 2, 5, 6, 7, 8, 9, 10, 13, 14];
    let bits = 4;
    let result = minimize(&minterms, bits);
    
    println!("4-variable function minimization:");
    for imp in &result {
        println!("{}", imp.to_string());
    }
}